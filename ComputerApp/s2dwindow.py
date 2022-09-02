import threading

import dearpygui.dearpygui as dpg
import numpy as np

import s2dutils
import s2dlog
import s2dfile


def run():
    s2d_logger = s2dlog.S2DLog()
    s2d_file = s2dfile.S2DFile()

    log_thread = threading.Thread(target=s2d_logger.listener, daemon=True)
    file_thread = threading.Thread(target=s2d_file.listener, daemon=True)

    log_thread.start()
    file_thread.start()

    dpg.create_context()
    dpg.configure_app(init_file="dpg.ini")
    # dpg.enable_docking()

    s2dutils.setup_fonts()

    # Create Primary Window
    dpg.add_window(tag="S2DCommunicator")

    log_view = dpg.add_window(tag="S2DLogView", label="S2D Logs", no_collapse=True, no_close=True)
    file_view = dpg.add_window(tag="S2DFileView", label="S2D Files", no_collapse=True, no_close=True)

    make_settings_window(s2d_logger, s2d_file)
    make_control_window(s2d_file)
    make_file_plot(file_view, s2d_file)

    dpg.create_viewport(title="S2D Communicator", width=1550, height=750)
    dpg.setup_dearpygui()
    dpg.show_viewport()
    dpg.set_exit_callback(exit_callback)
    dpg.set_primary_window("S2DCommunicator", True)

    while dpg.is_dearpygui_running():
        if not s2d_logger.text_queue.empty():
            log = s2d_logger.text_queue.get()
            color = s2d_logger.text_queue.get()
            add_text_log_view(log, color, log_view)

        if not s2d_file.data_queue.empty():
            data = s2d_file.data_queue.get()
            set_plot_file_view(data, s2d_file.series_tag)

        dpg.render_dearpygui_frame()

    dpg.destroy_context()


def make_file_plot(file_view, s2d_file):
    with dpg.plot(label="Line Series", height=490, width=875, parent=file_view):
        # optionally create legend
        # dpg.add_plot_legend()

        # REQUIRED: create x and y axes
        x_axis = dpg.add_plot_axis(dpg.mvXAxis, label="x", tag="XAxisFilePlot")
        y_axis = dpg.add_plot_axis(dpg.mvYAxis, label="y", tag="YAxisFilePlot")

        s2d_file.series_tag = dpg.add_line_series([], [], label="arm_sin", tag="ArmSinPlot", parent=y_axis)

        dpg.set_axis_limits(x_axis, 0, 2*np.pi)
        dpg.set_axis_limits(y_axis, -1, 1)


def signal_file_begin_transfer(sender, app_data, user_data):
    s2d_file = user_data

    s2d_file.length = int(dpg.get_value(s2d_file.size_tag))
    s2d_file.size = s2d_file.length * 4  # size of a float in bytes (on MCU and PC)
    s2d_file.freq = dpg.get_value(s2d_file.freq_tag)

    print(f"size={s2d_file.length}")
    print(f"freq={s2d_file.freq}")
    s2d_file.event.set()


def make_control_window(s2d_file):
    with dpg.window(tag="S2DFileControl", label="Control Panel", no_collapse=True, no_close=True):
        text = dpg.add_text("Size\n\nFreq")
        pos = dpg.get_item_pos(text)

        pos_x = pos[0] + 60
        pos_y = pos[1] + 38

        s2d_file.size_tag = dpg.add_combo(tag="RequestSampleSize", items=["128", "256"], default_value="128", width=175,
                                          pos=[pos_x, pos_y])
        pos_y += 42

        s2d_file.freq_tag = dpg.add_input_float(tag="RequestSampleFrequency", default_value=1000, width=175,
                                                pos=[pos_x, pos_y])
        pos_y += 42

        dpg.add_button(label="Request Payload", callback=signal_file_begin_transfer, user_data=s2d_file, width=175,
                       pos=[pos_x, pos_y])


def make_settings_window(s2d_logger, s2d_file):
    list_of_serial_ports = s2dutils.serial_ports()
    list_of_serial_ports.sort()

    with dpg.window(label="Port Settings", no_collapse=True, no_close=True):
        port_label_tag = dpg.add_text("Log Port\n\nFile Port\n\n")
        log_port_pos = dpg.get_item_pos(port_label_tag)

        log_port_x = log_port_pos[0]
        log_port_y = log_port_pos[1]

        log_ports_combo = dpg.add_combo(tag="LogSerialPortsList", pos=[log_port_x+120, log_port_y+37], width=175,
                                        items=list_of_serial_ports, default_value=list_of_serial_ports[0])

        file_ports_combo = dpg.add_combo(tag="FileSerialPortsList", pos=[log_port_x+120, log_port_y+81], width=175,
                                         items=list_of_serial_ports, default_value=list_of_serial_ports[0])

        ports_combo_pos = dpg.get_item_pos(log_ports_combo)
        pcp_x = ports_combo_pos[0]
        pcp_y = ports_combo_pos[1]

        log_speed_combo = dpg.add_combo(tag="LogBaudRateList", pos=[pcp_x+195, pcp_y], width=115,
                                        items=["115200", "9600"], default_value="115200")
        pcp_x += 195

        file_speed_combo = dpg.add_combo(tag="FileBaudRateList", pos=[pcp_x, pcp_y+45], width=115,
                                         items=["115200", "9600"], default_value="115200")
        pcp_x += 134

        dpg.add_button(tag="LogConnect", label="connect", pos=[pcp_x, pcp_y], width=125, callback=update_combos,
                       user_data=[log_ports_combo, log_speed_combo, file_ports_combo, s2d_logger])

        dpg.add_button(tag="FileConnect", label="connect", pos=[pcp_x, pcp_y+45], width=125,
                       callback=update_combos, user_data=[file_ports_combo, file_speed_combo,
                       log_ports_combo, s2d_file])

        # dpg.add_button(label="Refresh List", pos=[340, 37], width=136, callback=update_serial_ports_combo,
        #                user_data=ports_combo)


def update_combos(sender, app_data, user_data):
    port_tag = user_data[0]
    baud_tag = user_data[1]

    port = dpg.get_value(port_tag)
    baud = dpg.get_value(baud_tag)

    # either File or Log combo-box
    other_combo = user_data[2]
    other_combo_items = dpg.get_item_configuration(other_combo)["items"]
    other_combo_enabled = dpg.get_item_configuration(other_combo)["enabled"]

    s2d_obj = user_data[3]

    if dpg.get_item_label(sender) == "connect":
        dpg.configure_item(sender, label="disconnect")

        dpg.configure_item(port_tag, enabled=False, no_arrow_button=True)
        dpg.configure_item(baud_tag, enabled=False, no_arrow_button=True)

        if port in other_combo_items:
            other_combo_items.remove(port)
            other_combo_items.sort()

            if other_combo_enabled:
                dpg.configure_item(other_combo, items=other_combo_items, default_value=other_combo_items[0])
            else:
                dpg.configure_item(other_combo, items=other_combo_items)

        s2d_obj.port_config_queue.put(port)
        s2d_obj.port_config_queue.put(baud)

    else:
        dpg.configure_item(sender, label="connect")
        dpg.configure_item(port_tag, enabled=True, no_arrow_button=False)
        dpg.configure_item(baud_tag, enabled=True, no_arrow_button=False)

        if port not in other_combo_items:
            other_combo_items.append(port)
            other_combo_items.sort()

            if other_combo_enabled:
                dpg.configure_item(other_combo, items=other_combo_items, default_value=other_combo_items[0])
            else:
                dpg.configure_item(other_combo, items=other_combo_items)

        s2d_obj.disconnect_queue.put(1)


def update_serial_ports_combo(sender, app_data, user_data):
    list_of_serial_ports = s2dutils.serial_ports()
    dpg.configure_item(user_data, items=list_of_serial_ports)


def add_text_log_view(log, color, tag):
    dpg.add_text(log, parent=tag, color=color)
    dpg.get_item_parent(tag)
    dpg.set_y_scroll(tag, -1)

    # remove log items from window
    log_lines = dpg.get_item_info(tag)
    log_lines = log_lines['children'][1]

    # start deleting at an arbitrary number of logs
    if len(log_lines) > 512:
        print(f"delete log:{log_lines[0]}")
        dpg.delete_item(log_lines[0])


def set_plot_file_view(plot, tag):
    x = np.linspace(0, 2*np.pi, num=len(plot))
    dpg.set_value(tag, [x, plot])


def exit_callback():
    print("Exit called...")
    s2dutils.save_init()
