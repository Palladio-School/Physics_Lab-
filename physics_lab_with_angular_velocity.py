# ---------- IMPORTS ----------
import socket
import threading
import customtkinter as ctk
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2Tk
from collections import deque
import numpy as np
from tkinter import filedialog
from tkinter import ttk
import csv
import mplcursors
import time

# ---------- CONFIG ----------
UDP_IP = "0.0.0.0"
UDP_PORT = 4210
BUFFER_SIZE = 200
G = 9.81    # m/s²

# ---------- MAIN APP ----------
class M5App(ctk.CTk):
    def __init__(self):
        super().__init__()
        self.title("Palladio Physics Lab")
        self.geometry("1140x900")
        self.minsize(1200, 940)
        ctk.set_appearance_mode("dark")
        ctk.set_default_color_theme("blue")

        self.protocol("WM_DELETE_WINDOW", self.on_close)  # Handle window close

        self.container = ctk.CTkFrame(self)
        self.container.pack(fill="both", expand=True)

        self.frames = {}
        for Page in (StartPage, ForcePage, AngularVelocityPage, LoadCellForcePage, UltrasonicPage):
            frame = Page(parent=self.container, controller=self)
            self.frames[Page.__name__] = frame
            frame.grid(row=0, column=0, sticky="nsew")

        self.show_frame("StartPage")

    def show_frame(self, page_name):
        self.frames[page_name].tkraise()

    def on_close(self):
        self.destroy()
        exit(0)

# ---------- START PAGE ----------
class StartPage(ctk.CTkFrame):
    def __init__(self, parent, controller):
        super().__init__(parent)
        ctk.CTkLabel(self, text="Palladio Physics Labs", font=("Arial", 32)).pack(pady=40)
        ctk.CTkButton(self, text="Force (Accel)", font=("Arial", 20),
                      command=lambda: controller.show_frame("ForcePage")).pack(pady=20)
        ctk.CTkButton(self, text="Angular Velocity", font=("Arial", 20),
                      command=lambda: controller.show_frame("AngularVelocityPage")).pack(pady=20)
        ctk.CTkButton(self, text="Force Sensor (Load Cell)", font=("Arial", 20),
              command=lambda: controller.show_frame("LoadCellForcePage")).pack(pady=20)
        ctk.CTkButton(self, text="Ultrasonic Motion", font=("Arial", 20),
              command=lambda: controller.show_frame("UltrasonicPage")).pack(pady=20)
        ctk.CTkButton(self, text="Exit", font=("Arial", 20), command=controller.on_close).pack(pady=20)
        
# ---------- ForcePage ----------
class ForcePage(ctk.CTkFrame):
    def __init__(self, parent, controller):
        super().__init__(parent)
        self.controller = controller
        self.running = False
        self.last_battery_update = 0
        self.mass = 1.0
        self.area_enabled = True

        ctk.CTkButton(self, text="← Back", command=lambda: controller.show_frame("StartPage")).pack(anchor="nw", padx=10, pady=10)

        self.left_frame = ctk.CTkFrame(self, width=200)
        self.left_frame.pack(side="left", fill="y", padx=10, pady=10)

        self.right_frame = ctk.CTkFrame(self)
        self.right_frame.pack(side="right", expand=True, fill="both", padx=10, pady=10)

        self.label_x = ctk.CTkLabel(self.left_frame, text="X: 0.00 m/s²", font=("Arial", 20))
        self.label_y = ctk.CTkLabel(self.left_frame, text="Y: 0.00 m/s²", font=("Arial", 20))
        self.label_z = ctk.CTkLabel(self.left_frame, text="Z: 0.00 m/s²", font=("Arial", 20))
        self.voltage_label = ctk.CTkLabel(self.left_frame, text="Battery: 0.00V", font=("Arial", 18))
        self.voltage_bar = ctk.CTkProgressBar(self.left_frame, width=130)
        self.status = ctk.CTkLabel(self.left_frame, text="Waiting for data...", text_color="gray")
        self.toggle_button = ctk.CTkButton(self.left_frame, text="Start", command=self.toggle_data)
        self.save_button = ctk.CTkButton(self.left_frame, text="Export CSV", command=self.export_csv)
        self.clear_button = ctk.CTkButton(self.left_frame, text="Clear Data", command=self.clear_data)
        self.calc_area_button = ctk.CTkButton(self.left_frame, text="Calculate Impulse", command=self.calculate_impulse)
        self.clear_area_button = ctk.CTkButton(self.left_frame, text="Clear Area", command=self.clear_shaded_area)
        self.toggle_area_button = ctk.CTkButton(self.left_frame, text="Disable Area Tool", command=self.toggle_area_tool)
        self.area_result_label = ctk.CTkLabel(self.left_frame, text="", font=("Arial", 20), text_color="orange")

        for widget in [self.label_x, self.label_y, self.label_z, self.voltage_label, self.voltage_bar,
                       self.status, self.toggle_button, self.save_button, self.clear_button,
                       self.calc_area_button, self.clear_area_button, self.toggle_area_button, self.area_result_label]:
            widget.pack(pady=5)

        self.ax_data = deque([0]*BUFFER_SIZE, maxlen=BUFFER_SIZE)
        self.ay_data = deque([0]*BUFFER_SIZE, maxlen=BUFFER_SIZE)
        self.az_data = deque([0]*BUFFER_SIZE, maxlen=BUFFER_SIZE)
        self.force_data = deque([0]*BUFFER_SIZE, maxlen=BUFFER_SIZE)
        self.time_data = deque(np.linspace(-BUFFER_SIZE/50, 0, BUFFER_SIZE), maxlen=BUFFER_SIZE)
        self.sample_count = 0

        self.fig, self.axes = plt.subplots(1, 2, figsize=(10, 4), dpi=100)
        self.fig.tight_layout(pad=4)
        bg_color = "#212121"
        fg_color = "white"
        self.fig.patch.set_facecolor(bg_color)
        for ax in self.axes:
            ax.set_facecolor(bg_color)
            ax.tick_params(axis='x', colors=fg_color)
            ax.tick_params(axis='y', colors=fg_color)
            ax.title.set_color(fg_color)
            ax.xaxis.label.set_color(fg_color)
            ax.yaxis.label.set_color(fg_color)
            for spine in ax.spines.values():
                spine.set_color(fg_color)

        self.plot_ax, = self.axes[0].plot([], [], color='red', label="X")
        self.plot_ay, = self.axes[0].plot([], [], color='green', label="Y")
        self.plot_az, = self.axes[0].plot([], [], color='blue', label="Z")
        self.axes[0].set_title("Acceleration (m/s²)")
        self.axes[0].legend(loc='upper right')

        self.plot_force, = self.axes[1].plot([], [], color='orange', label="Force")
        self.axes[1].set_title("Force Magnitude (N)")

        self.canvas = FigureCanvasTkAgg(self.fig, master=self.right_frame)
        self.canvas_widget = self.canvas.get_tk_widget()
        self.canvas_widget.pack(fill="both", expand=True)

        self.toolbar = NavigationToolbar2Tk(self.canvas, self.right_frame)
        self.toolbar.update()
        self.toolbar.pack(side="top", fill="x")

        self.mass_input_frame = ctk.CTkFrame(self.right_frame, fg_color="transparent")
        self.mass_input_frame.pack(fill="x", pady=(5, 5))
        ctk.CTkLabel(self.mass_input_frame, text="Mass (kg):", font=("Arial", 16)).pack(side="left", padx=10)
        self.mass_entry = ctk.CTkEntry(self.mass_input_frame, width=100)
        self.mass_entry.insert(0, "1.0")
        self.mass_entry.pack(side="left", padx=5)
        ctk.CTkButton(self.mass_input_frame, text="Set Mass", command=self.set_mass).pack(side="left", padx=10)

        style = ttk.Style()
        style.theme_use("default")
        style.configure("Treeview",
                        background="#2b2b2b",
                        foreground="white",
                        fieldbackground="#2b2b2b",
                        rowheight=24,
                        font=('Arial', 12))
        style.configure("Treeview.Heading",
                        background="#1f1f1f",
                        foreground="white",
                        font=('Arial', 13, 'bold'))
        style.map("Treeview", background=[('selected', '#3a3a3a')])

        self.tree = ttk.Treeview(self.right_frame, columns=("time", "ax", "ay", "az", "force"), show="headings", height=8)
        for col in ("time", "ax", "ay", "az", "force"):
            self.tree.heading(col, text=col.replace("_", " ").title())
            self.tree.column(col, anchor="center", width=100)
        self.tree.pack(fill="x")

        self.cursor = mplcursors.cursor([self.plot_ax, self.plot_ay, self.plot_az, self.plot_force], hover=False)
        self.cursor.connect("add", self.on_cursor_add)

        self.selected_points = []
        self.shaded_patch = None
        self.vlines = []
        self.canvas.mpl_connect("button_press_event", self.on_click)

        self.start_udp_thread()

    def toggle_area_tool(self):
        self.area_enabled = not self.area_enabled
        if self.area_enabled:
            self.toggle_area_button.configure(text="Disable Area Tool")
            self.status.configure(text="Area tool enabled", text_color="green")
        else:
            self.toggle_area_button.configure(text="Enable Area Tool")
            self.status.configure(text="Area tool disabled", text_color="gray")

    def on_click(self, event):
        if not self.area_enabled or event.inaxes != self.axes[1]:
            return
        self.cursor.enabled = False
        if len(self.selected_points) < 2:
            self.selected_points.append(event.xdata)
            vline = self.axes[1].axvline(event.xdata, color='white', linestyle='--', alpha=0.5)
            self.vlines.append(vline)
            self.canvas.draw()
        self.cursor.enabled = True

    def calculate_impulse(self):
        if len(self.selected_points) < 2:
            self.status.configure(text="Select two points first", text_color="orange")
            return
        x1, x2 = sorted(self.selected_points)
        t = np.array(self.time_data)
        f = np.array(self.force_data)
        mask = (t >= x1) & (t <= x2)
        t_range = t[mask]
        f_range = f[mask]
        if len(t_range) < 2:
            self.area_result_label.configure(text="")
            return
        if self.shaded_patch:
            self.shaded_patch.remove()
        self.shaded_patch = self.axes[1].fill_between(t_range, f_range, color="orange", alpha=0.4)
        auc = np.trapz(f_range, t_range)
        self.area_result_label.configure(text=f"Impulse: {auc:.2f} N·s")
        self.canvas.draw()

    def clear_shaded_area(self):
        if self.shaded_patch:
            self.shaded_patch.remove()
            self.shaded_patch = None
        for vline in self.vlines:
            vline.remove()
        self.vlines.clear()
        self.area_result_label.configure(text="")
        self.canvas.draw()
        self.selected_points = []

    def set_mass(self):
        try:
            self.mass = float(self.mass_entry.get())
        except ValueError:
            self.mass_entry.configure(placeholder_text="Invalid")

    def toggle_data(self):
        self.running = not self.running
        self.toggle_button.configure(text="Stop" if self.running else "Start")
        self.status.configure(
            text="Receiving data..." if self.running else "Paused",
            text_color="green" if self.running else "orange"
        )

    def start_udp_thread(self):
        threading.Thread(target=self.udp_listener, daemon=True).start()

    def udp_listener(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.bind((UDP_IP, UDP_PORT))
        while True:
            try:
                data, _ = sock.recvfrom(1024)
                parts = data.decode().strip().split(",")
                if len(parts) == 4:
                    ax, ay, az, voltage = map(float, parts)
                    self.process_data(ax, ay, az, voltage)
                elif len(parts) == 3:
                    ax, ay, az = map(float, parts)
                    self.process_data(ax, ay, az, None)
            except Exception as e:
                print("Error:", e)

    def process_data(self, ax, ay, az, voltage):
        ax_ms2 = ax * G
        ay_ms2 = ay * G
        az_ms2 = az * G
        self.label_x.configure(text=f"X: {ax_ms2:.2f} m/s²")
        self.label_y.configure(text=f"Y: {ay_ms2:.2f} m/s²")
        self.label_z.configure(text=f"Z: {az_ms2:.2f} m/s²")
        if voltage is not None and time.time() - self.last_battery_update >= 60:
            percent = (voltage - 3.3) / (4.2 - 3.3)
            percent = max(0.0, min(percent, 1.0))
            self.voltage_label.configure(text=f"Battery ({int(percent * 100)}%)")
            self.voltage_bar.set(percent)
            self.last_battery_update = time.time()
        if not self.running:
            return
        self.sample_count += 1
        timestamp = self.sample_count / 50.0
        self.time_data.append(timestamp)
        self.ax_data.append(ax_ms2)
        self.ay_data.append(ay_ms2)
        self.az_data.append(az_ms2)
        force = np.sqrt(ax_ms2**2 + ay_ms2**2 + az_ms2**2) * self.mass
        self.force_data.append(force)
        self.plot_ax.set_data(self.time_data, self.ax_data)
        self.plot_ay.set_data(self.time_data, self.ay_data)
        self.plot_az.set_data(self.time_data, self.az_data)
        self.plot_force.set_data(self.time_data, self.force_data)
        self.axes[0].relim(); self.axes[0].autoscale_view()
        self.axes[1].relim(); self.axes[1].autoscale_view()
        self.canvas.draw()
        self.tree.insert("", 0, values=(f"{timestamp:.2f}", f"{ax_ms2:.2f}", f"{ay_ms2:.2f}", f"{az_ms2:.2f}", f"{force:.2f}"))
        if len(self.tree.get_children()) > BUFFER_SIZE:
            self.tree.delete(self.tree.get_children()[-1])

    def export_csv(self):
        file = filedialog.asksaveasfilename(defaultextension=".csv")
        if file:
            with open(file, 'w', newline='') as f:
                writer = csv.writer(f)
                writer.writerow(["Time", "Ax", "Ay", "Az", "Force"])
                for t, x, y, z, f_ in zip(self.time_data, self.ax_data, self.ay_data, self.az_data, self.force_data):
                    writer.writerow([t, x, y, z, f_])

    def clear_data(self):
        self.ax_data.clear()
        self.ay_data.clear()
        self.az_data.clear()
        self.force_data.clear()
        self.time_data.clear()
        self.sample_count = 0
        self.plot_ax.set_data([], [])
        self.plot_ay.set_data([], [])
        self.plot_az.set_data([], [])
        self.plot_force.set_data([], [])
        self.canvas.draw()
        for row in self.tree.get_children():
            self.tree.delete(row)

    def on_cursor_add(self, sel):
        if not self.area_enabled:
            return
        idx = int(sel.index)
        t = self.time_data[idx]
        y = sel.target[1]
        label = sel.artist.get_label()
        sel.annotation.set_text(f"t={t:.2f}s\n{label} = {y:.2f} {'m/s²' if label in ['X','Y','Z'] else 'N'}")

# ---------- ANGULAR VELOCITY PAGE ----------
class AngularVelocityPage(ctk.CTkFrame):
    def __init__(self, parent, controller):
        super().__init__(parent)
        self.controller = controller
        self.running = False
        self.sample_count = 0
        self.wx_data = deque([0]*BUFFER_SIZE, maxlen=BUFFER_SIZE)
        self.wy_data = deque([0]*BUFFER_SIZE, maxlen=BUFFER_SIZE)
        self.wz_data = deque([0]*BUFFER_SIZE, maxlen=BUFFER_SIZE)
        self.zonly_data = deque([0]*BUFFER_SIZE, maxlen=BUFFER_SIZE)
        self.time_data = deque(np.linspace(-BUFFER_SIZE/50, 0, BUFFER_SIZE), maxlen=BUFFER_SIZE)

        ctk.CTkButton(self, text="← Back", command=lambda: controller.show_frame("StartPage")).pack(anchor="nw", padx=10, pady=10)

        self.left_frame = ctk.CTkFrame(self, width=200)
        self.left_frame.pack(side="left", fill="y", padx=10, pady=10)

        self.right_frame = ctk.CTkFrame(self)
        self.right_frame.pack(side="right", expand=True, fill="both", padx=10, pady=10)

        self.label_wx = ctk.CTkLabel(self.left_frame, text="ωx:  00.00 rad/s", font=("Courier", 20))
        self.label_wy = ctk.CTkLabel(self.left_frame, text="ωy:  00.00 rad/s", font=("Courier", 20))
        self.label_wz = ctk.CTkLabel(self.left_frame, text="ωz:  00.00 rad/s", font=("Courier", 20))
        self.voltage_label = ctk.CTkLabel(self.left_frame, text="Battery: 0.00V", font=("Arial", 18))
        self.voltage_bar = ctk.CTkProgressBar(self.left_frame, width=130)
        self.status = ctk.CTkLabel(self.left_frame, text="Waiting for angular velocity data...", text_color="gray")
        self.toggle_button = ctk.CTkButton(self.left_frame, text="Start", command=self.toggle_data)
        self.export_button = ctk.CTkButton(self.left_frame, text="Export CSV", command=self.export_csv)
        self.clear_button = ctk.CTkButton(self.left_frame, text="Clear Data", command=self.clear_data)

        for widget in [self.label_wx, self.label_wy, self.label_wz, self.voltage_label, self.voltage_bar,
                       self.status, self.toggle_button, self.export_button, self.clear_button]:
            widget.pack(pady=5)

        # --- Matplotlib Plots ---
        self.fig, self.axes = plt.subplots(1, 2, figsize=(10, 4), dpi=100)
        self.fig.tight_layout(pad=4)
        bg_color = "#212121"
        fg_color = "white"
        self.fig.patch.set_facecolor(bg_color)
        for ax in self.axes:
            ax.set_facecolor(bg_color)
            ax.tick_params(axis='x', colors=fg_color)
            ax.tick_params(axis='y', colors=fg_color)
            ax.title.set_color(fg_color)
            ax.xaxis.label.set_color(fg_color)
            ax.yaxis.label.set_color(fg_color)
            for spine in ax.spines.values():
                spine.set_color(fg_color)

        self.line_wx, = self.axes[0].plot([], [], color='red', label='ωx')
        self.line_wy, = self.axes[0].plot([], [], color='green', label='ωy')
        self.line_wz, = self.axes[0].plot([], [], color='blue', label='ωz')
        self.axes[0].set_title("Angular Velocity XYZ (rad/s)")
        self.axes[0].legend(loc='upper right')

        self.line_wz_only, = self.axes[1].plot([], [], color='orange', label='ωz only')
        self.axes[1].set_title("Angular Velocity Z (rad/s)")
        self.axes[1].legend(loc='upper right')

        self.canvas = FigureCanvasTkAgg(self.fig, master=self.right_frame)
        self.canvas_widget = self.canvas.get_tk_widget()
        self.canvas_widget.pack(fill="both", expand=True)
        self.canvas_widget.configure(height=400)

        self.toolbar = NavigationToolbar2Tk(self.canvas, self.right_frame)
        self.toolbar.update()
        self.toolbar.pack(side="top", fill="x")

        # Safe styling of toolbar widgets
        for child in self.toolbar.winfo_children():
            try:
                child.configure(bg="#2b2b2b", fg="white", highlightthickness=0)
            except:
                pass

        # --- Table below graphs ---
        style = ttk.Style()
        style.theme_use("default")
        style.configure("Treeview",
                        background="#2b2b2b",
                        foreground="white",
                        fieldbackground="#2b2b2b",
                        rowheight=24,
                        font=('Arial', 12))
        style.configure("Treeview.Heading",
                        background="#1f1f1f",
                        foreground="white",
                        font=('Arial', 13, 'bold'))
        style.map("Treeview", background=[('selected', '#3a3a3a')])

        self.tree = ttk.Treeview(self.right_frame, columns=("time", "wx", "wy", "wz"), show="headings", height=8)
        for col in ("time", "wx", "wy", "wz"):
            self.tree.heading(col, text=col.upper())
            self.tree.column(col, anchor="center", width=100)
        self.tree.pack(fill="x", pady=(5, 10))

        self.start_udp_thread()

    def toggle_data(self):
        self.running = not self.running
        self.toggle_button.configure(text="Stop" if self.running else "Start")
        self.status.configure(
            text="Receiving data..." if self.running else "Paused",
            text_color="green" if self.running else "orange"
        )

    def start_udp_thread(self):
        threading.Thread(target=self.udp_listener, daemon=True).start()

    def udp_listener(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.bind((UDP_IP, UDP_PORT + 1))
        while True:
            try:
                data, _ = sock.recvfrom(1024)
                parts = data.decode().strip().split(",")
                if len(parts) == 4:
                    wx, wy, wz, voltage = map(float, parts)
                    self.process_data(wx, wy, wz, voltage)
                elif len(parts) == 3:
                    wx, wy, wz = map(float, parts)
                    self.process_data(wx, wy, wz, None)
            except Exception as e:
                print("Angular velocity error:", e)

    def process_data(self, wx, wy, wz, voltage):
        self.label_wx.configure(text=f"ωx: {wx:>6.2f} rad/s")
        self.label_wy.configure(text=f"ωy: {wy:>6.2f} rad/s")
        self.label_wz.configure(text=f"ωz: {wz:>6.2f} rad/s")

        if voltage is not None and time.time() - getattr(self, 'last_battery_update', 0) >= 60:
            percent = (voltage - 3.3) / (4.2 - 3.3)
            percent = max(0.0, min(percent, 1.0))
            self.voltage_label.configure(text=f"Battery ({int(percent * 100)}%)")
            self.voltage_bar.set(percent)
            self.last_battery_update = time.time()

        if not self.running:
            return

        self.sample_count += 1
        timestamp = self.sample_count / 50.0
        self.time_data.append(timestamp)
        self.wx_data.append(wx)
        self.wy_data.append(wy)
        self.wz_data.append(wz)
        self.zonly_data.append(wz)

        self.line_wx.set_data(self.time_data, self.wx_data)
        self.line_wy.set_data(self.time_data, self.wy_data)
        self.line_wz.set_data(self.time_data, self.wz_data)
        self.line_wz_only.set_data(self.time_data, self.zonly_data)

        self.axes[0].relim(); self.axes[0].autoscale_view()
        self.axes[1].relim(); self.axes[1].autoscale_view()
        self.canvas.draw()

        self.tree.insert("", 0, values=(f"{timestamp:.2f}", f"{wx:.2f}", f"{wy:.2f}", f"{wz:.2f}"))
        if len(self.tree.get_children()) > BUFFER_SIZE:
            self.tree.delete(self.tree.get_children()[-1])
    def export_csv(self):
        file = filedialog.asksaveasfilename(defaultextension=".csv")
        if file:
            with open(file, 'w', newline='') as f:
                writer = csv.writer(f)
                writer.writerow(["Time", "ωx", "ωy", "ωz"])
                for t, x, y, z in zip(self.time_data, self.wx_data, self.wy_data, self.wz_data):
                    writer.writerow([t, x, y, z])

    def clear_data(self):
        self.wx_data.clear()
        self.wy_data.clear()
        self.wz_data.clear()
        self.zonly_data.clear()
        self.time_data.clear()
        self.sample_count = 0
        self.line_wx.set_data([], [])
        self.line_wy.set_data([], [])
        self.line_wz.set_data([], [])
        self.line_wz_only.set_data([], [])
        self.canvas.draw()
        for row in self.tree.get_children():
            self.tree.delete(row)
            
            
# -----------LOADCELL PAGE-----------
class LoadCellForcePage(ctk.CTkFrame):
    def __init__(self, parent, controller):
        super().__init__(parent)
        self.controller = controller
        self.running = False
        self.sample_count = 0
        self.force_data = deque([0]*BUFFER_SIZE, maxlen=BUFFER_SIZE)
        self.time_data = deque(np.linspace(-BUFFER_SIZE/1, 0, BUFFER_SIZE), maxlen=BUFFER_SIZE)

        self.voltage = 0
        self.last_battery_update = 0

        ctk.CTkButton(self, text="← Back", command=lambda: controller.show_frame("StartPage")).pack(anchor="nw", padx=10, pady=10)

        self.left_frame = ctk.CTkFrame(self, width=200)
        self.left_frame.pack(side="left", fill="y", padx=10, pady=10)

        self.right_frame = ctk.CTkFrame(self)
        self.right_frame.pack(side="right", expand=True, fill="both", padx=10, pady=10)

        self.force_label = ctk.CTkLabel(self.left_frame, text="Force: 0.00 N", font=("Arial", 24))
        self.voltage_label = ctk.CTkLabel(self.left_frame, text="Battery: 0.00V", font=("Arial", 18))
        self.voltage_bar = ctk.CTkProgressBar(self.left_frame, width=130)
        self.status = ctk.CTkLabel(self.left_frame, text="Waiting for load cell data...", text_color="gray")
        self.toggle_button = ctk.CTkButton(self.left_frame, text="Start", command=self.toggle_data)
        self.export_button = ctk.CTkButton(self.left_frame, text="Export CSV", command=self.export_csv)
        self.clear_button = ctk.CTkButton(self.left_frame, text="Clear Data", command=self.clear_data)

        for widget in [self.force_label, self.voltage_label, self.voltage_bar, self.status,
                       self.toggle_button, self.export_button, self.clear_button]:
            widget.pack(pady=5)

        self.fig, self.ax = plt.subplots(figsize=(6, 3), dpi=100)
        self.fig.tight_layout(pad=4)
        self.fig.patch.set_facecolor("#212121")
        self.ax.set_facecolor("#212121")
        self.ax.tick_params(axis='x', colors="white")
        self.ax.tick_params(axis='y', colors="white")
        self.ax.title.set_color("white")
        self.ax.xaxis.label.set_color("white")
        self.ax.yaxis.label.set_color("white")
        for spine in self.ax.spines.values():
            spine.set_color("white")

        self.line, = self.ax.plot([], [], color='orange', label="Force")
        self.ax.set_title("Load Cell Force (N)")
        self.ax.legend(loc='upper right')

        self.canvas = FigureCanvasTkAgg(self.fig, master=self.right_frame)
        self.canvas_widget = self.canvas.get_tk_widget()
        self.canvas_widget.pack(fill="both", expand=True)

        self.toolbar = NavigationToolbar2Tk(self.canvas, self.right_frame)
        self.toolbar.update()
        self.toolbar.pack(side="top", fill="x")

        self.tree = ttk.Treeview(self.right_frame, columns=("time", "force"), show="headings", height=8)
        self.tree.heading("time", text="Time (s)")
        self.tree.heading("force", text="Force (N)")
        self.tree.column("time", anchor="center", width=100)
        self.tree.column("force", anchor="center", width=100)
        self.tree.pack(fill="x")

        self.annotation = None
        self.vline = None
        self.canvas.mpl_connect("button_press_event", self.on_click)

        self.start_udp_thread()

    def on_click(self, event):
        if event.inaxes != self.ax:
            return

        # Find nearest index in time_data
        times = np.array(self.time_data)
        if len(times) == 0:
            return
        idx = (np.abs(times - event.xdata)).argmin()

        # Remove previous annotation and vline
        if self.annotation:
            self.annotation.remove()
        if self.vline:
            self.vline.remove()

        # Add new annotation and vertical line
        self.annotation = self.ax.annotate(
            f"t = {self.time_data[idx]:.2f}s\nF = {self.force_data[idx]:.2f} N",
            xy=(self.time_data[idx], self.force_data[idx]),
            xytext=(10, 20),
            textcoords="offset points",
            fontsize=10,
            color='orange',
            bbox=dict(boxstyle="round,pad=0.3", fc="black", ec="orange", lw=1)
        )
        self.vline = self.ax.axvline(self.time_data[idx], color='orange', linestyle='--', alpha=0.7)

        self.canvas.draw()

    def toggle_data(self):
        self.running = not self.running
        self.toggle_button.configure(text="Stop" if self.running else "Start")
        self.status.configure(text="Receiving data..." if self.running else "Paused",
                              text_color="green" if self.running else "orange")

    def start_udp_thread(self):
        threading.Thread(target=self.udp_listener, daemon=True).start()

    def udp_listener(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.bind((UDP_IP, 4213))
        while True:
            try:
                data, _ = sock.recvfrom(1024)
                parts = data.decode().strip().split(",")
                if len(parts) == 2:
                    force, voltage = map(float, parts)
                    self.process_data(force, voltage)
            except Exception as e:
                print("Load cell error:", e)

    def process_data(self, force, voltage):
        self.force_label.configure(text=f"Force: {force:.2f} N")

        if voltage is not None and time.time() - self.last_battery_update >= 60:
            percent = (voltage - 3.3) / (4.2 - 3.3)
            percent = max(0.0, min(percent, 1.0))
            self.voltage_label.configure(text=f"Battery ({int(percent * 100)}%)")
            self.voltage_bar.set(percent)
            self.last_battery_update = time.time()

        if not self.running:
            return

        self.sample_count += 1
        timestamp = self.sample_count / 1.0
        self.time_data.append(timestamp)
        self.force_data.append(force)

        self.line.set_data(self.time_data, self.force_data)
        self.ax.relim()
        self.ax.autoscale_view()
        self.canvas.draw()

        self.tree.insert("", 0, values=(f"{timestamp:.2f}", f"{force:.2f}"))
        if len(self.tree.get_children()) > BUFFER_SIZE:
            self.tree.delete(self.tree.get_children()[-1])

    def export_csv(self):
        file = filedialog.asksaveasfilename(defaultextension=".csv")
        if file:
            with open(file, 'w', newline='') as f:
                writer = csv.writer(f)
                writer.writerow(["Time", "Force"])
                for t, f_ in zip(self.time_data, self.force_data):
                    writer.writerow([t, f_])

    def clear_data(self):
        self.force_data.clear()
        self.time_data.clear()
        self.sample_count = 0
        self.line.set_data([], [])
        if self.annotation:
            self.annotation.remove()
            self.annotation = None
        if self.vline:
            self.vline.remove()
            self.vline = None
        self.canvas.draw()
        for row in self.tree.get_children():
            self.tree.delete(row)

# ---------- ULTRASONIC PAGE ----------
class UltrasonicPage(ctk.CTkFrame):
    def __init__(self, parent, controller):
        super().__init__(parent)
        self.controller = controller
        self.running = False
        self.sample_count = 0

        self.time_data = deque([0]*BUFFER_SIZE, maxlen=BUFFER_SIZE)
        self.pos_data = deque([0]*BUFFER_SIZE, maxlen=BUFFER_SIZE)
        self.vel_data = deque([0]*BUFFER_SIZE, maxlen=BUFFER_SIZE)
        self.acc_data = deque([0]*BUFFER_SIZE, maxlen=BUFFER_SIZE)

        # Top left: Controls
        ctk.CTkButton(self, text="← Back", command=lambda: controller.show_frame("StartPage")).pack(anchor="nw", padx=10, pady=10)

        self.left_frame = ctk.CTkFrame(self, width=200)
        self.left_frame.pack(side="left", fill="y", padx=10, pady=10)

        self.right_frame = ctk.CTkFrame(self)
        self.right_frame.pack(side="right", expand=True, fill="both", padx=10, pady=10)

        self.status_label = ctk.CTkLabel(self.left_frame, text="Waiting for ultrasonic data...", text_color="gray")
        self.toggle_button = ctk.CTkButton(self.left_frame, text="Start", command=self.toggle_data)
        self.clear_button = ctk.CTkButton(self.left_frame, text="Clear Data", command=self.clear_data)

        self.label_d = ctk.CTkLabel(self.left_frame, text="d: 0.00 m", font=("Arial", 20))
        self.label_v = ctk.CTkLabel(self.left_frame, text="v: 0.00 m/s", font=("Arial", 20))
        self.label_a = ctk.CTkLabel(self.left_frame, text="a: 0.00 m/s²", font=("Arial", 20))

        for w in [self.label_d, self.label_v, self.label_a, self.status_label, self.toggle_button, self.clear_button]:
            w.pack(pady=5)

        # Plotting
        self.fig, self.axes = plt.subplots(3, 1, figsize=(6, 6), dpi=100)
        self.fig.tight_layout(pad=4)
        bg_color = "#212121"
        fg_color = "white"

        self.fig.patch.set_facecolor(bg_color)
        for ax in self.axes:
            ax.set_facecolor(bg_color)
            ax.tick_params(axis='x', colors=fg_color)
            ax.tick_params(axis='y', colors=fg_color)
            ax.title.set_color(fg_color)
            ax.xaxis.label.set_color(fg_color)
            ax.yaxis.label.set_color(fg_color)
            for spine in ax.spines.values():
                spine.set_color(fg_color)

        self.line_d, = self.axes[0].plot([], [], label="Position (m)", color="orange")
        self.line_v, = self.axes[1].plot([], [], label="Velocity (m/s)", color="cyan")
        self.line_a, = self.axes[2].plot([], [], label="Acceleration (m/s²)", color="lime")

        self.axes[0].set_title("Position (m)")
        self.axes[1].set_title("Velocity (m/s)")
        self.axes[2].set_title("Acceleration (m/s²)")

        for ax in self.axes:
            ax.legend(loc='upper right')

        self.canvas = FigureCanvasTkAgg(self.fig, master=self.right_frame)
        self.canvas_widget = self.canvas.get_tk_widget()
        self.canvas_widget.pack(fill="both", expand=True)

        self.toolbar = NavigationToolbar2Tk(self.canvas, self.right_frame)
        self.toolbar.update()
        self.toolbar.pack(side="top", fill="x")

        # Table below
        style = ttk.Style()
        style.theme_use("default")
        style.configure("Treeview",
                        background="#2b2b2b",
                        foreground="white",
                        fieldbackground="#2b2b2b",
                        rowheight=24,
                        font=('Arial', 12))
        style.configure("Treeview.Heading",
                        background="#1f1f1f",
                        foreground="white",
                        font=('Arial', 13, 'bold'))

        self.tree = ttk.Treeview(self.right_frame, columns=("time", "pos", "vel", "acc"), show="headings", height=8)
        for col in ("time", "pos", "vel", "acc"):
            self.tree.heading(col, text=col.upper())
            self.tree.column(col, anchor="center", width=100)
        self.tree.pack(fill="x", pady=(5, 10))

        self.start_udp_thread()

    def toggle_data(self):
        self.running = not self.running
        self.toggle_button.configure(text="Stop" if self.running else "Start")
        self.status_label.configure(text="Receiving data..." if self.running else "Paused",
                                    text_color="green" if self.running else "orange")

    def start_udp_thread(self):
        threading.Thread(target=self.udp_listener, daemon=True).start()

    def udp_listener(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.bind((UDP_IP, 4215))
        while True:
            try:
                data, _ = sock.recvfrom(1024)
                parts = list(map(float, data.decode().strip().split(",")))
                if len(parts) == 3:
                    distance, velocity, acceleration = parts
                    self.process_data(distance, velocity, acceleration)
            except Exception as e:
                print("Ultrasonic error:", e)

    def process_data(self, d, v, a):
        self.label_d.configure(text=f"d: {d:.2f} m")
        self.label_v.configure(text=f"v: {v:.2f} m/s")
        self.label_a.configure(text=f"a: {a:.2f} m/s²")

        if not self.running:
            return

        self.sample_count += 1
        timestamp = self.sample_count / 20.0  # ~20Hz for ultrasonic

        self.time_data.append(timestamp)
        self.pos_data.append(d)
        self.vel_data.append(v)
        self.acc_data.append(a)

        self.line_d.set_data(self.time_data, self.pos_data)
        self.line_v.set_data(self.time_data, self.vel_data)
        self.line_a.set_data(self.time_data, self.acc_data)

        for ax in self.axes:
            ax.relim()
            ax.autoscale_view()

        self.canvas.draw()

        self.tree.insert("", 0, values=(f"{timestamp:.2f}", f"{d:.2f}", f"{v:.2f}", f"{a:.2f}"))
        if len(self.tree.get_children()) > BUFFER_SIZE:
            self.tree.delete(self.tree.get_children()[-1])

    def clear_data(self):
        self.time_data.clear()
        self.pos_data.clear()
        self.vel_data.clear()
        self.acc_data.clear()
        self.sample_count = 0
        self.line_d.set_data([], [])
        self.line_v.set_data([], [])
        self.line_a.set_data([], [])
        self.canvas.draw()
        for row in self.tree.get_children():
            self.tree.delete(row)

# ---------- RUN ----------
if __name__ == "__main__":
    app = M5App()
    app.mainloop()
    def export_csv(self):
        file = filedialog.asksaveasfilename(defaultextension=".csv")
        if file:
            with open(file, 'w', newline='') as f:
                writer = csv.writer(f)
                writer.writerow(["Time", "ωx", "ωy", "ωz"])
                for t, x, y, z in zip(self.time_data, self.wx_data, self.wy_data, self.wz_data):
                    writer.writerow([t, x, y, z])

    def clear_data(self):
        self.wx_data.clear()
        self.wy_data.clear()
        self.wz_data.clear()
        self.zonly_data.clear()
        self.time_data.clear()
        self.sample_count = 0
        self.line_wx.set_data([], [])
        self.line_wy.set_data([], [])
        self.line_wz.set_data([], [])
        self.line_wz_only.set_data([], [])
        self.canvas.draw()
        for row in self.tree.get_children():
            self.tree.delete(row)

# ---------- RUN ----------
if __name__ == "__main__":
    app = M5App()
    app.mainloop()
