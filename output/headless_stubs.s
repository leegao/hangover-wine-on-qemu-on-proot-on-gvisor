.text
.globl dummy_stub
dummy_stub: mov w0, #0; ret

.globl sane_init, sane_exit, sane_get_devices, sane_open, sane_close
.globl sane_get_option_descriptor, sane_control_option, sane_get_parameters
.globl sane_start, sane_snap, sane_read, sane_set_io_mode, sane_get_select_fd
.globl sane_strstatus, sane_cancel
sane_init: sane_exit: sane_get_devices: sane_open: sane_close:
sane_get_option_descriptor: sane_control_option: sane_get_parameters:
sane_start: sane_snap: sane_read: sane_set_io_mode: sane_get_select_fd:
sane_strstatus: sane_cancel: b dummy_stub

.globl snd_pcm_open, snd_pcm_close, snd_pcm_hw_params_any, snd_pcm_hw_params_set_access
.globl snd_pcm_hw_params_set_format, snd_pcm_hw_params_set_channels, snd_pcm_hw_params_set_rate_near
.globl snd_pcm_hw_params_set_buffer_time_near, snd_pcm_hw_params_set_period_time_near
.globl snd_pcm_hw_params, snd_pcm_prepare, snd_pcm_start, snd_pcm_drop, snd_pcm_drain
.globl snd_pcm_writei, snd_pcm_readi, snd_pcm_avail_update, snd_pcm_state
snd_pcm_open: snd_pcm_close: snd_pcm_hw_params_any: snd_pcm_hw_params_set_access:
snd_pcm_hw_params_set_format: snd_pcm_hw_params_set_channels: snd_pcm_hw_params_set_rate_near:
snd_pcm_hw_params_set_buffer_time_near: snd_pcm_hw_params_set_period_time_near:
snd_pcm_hw_params: snd_pcm_prepare: snd_pcm_start: snd_pcm_drop: snd_pcm_drain:
snd_pcm_writei: snd_pcm_readi: snd_pcm_avail_update: snd_pcm_state: b dummy_stub

.globl gst_init_check, gst_element_factory_make, gst_pipeline_new, gst_bin_add
.globl gst_element_set_state, gst_element_get_state, gst_object_unref
gst_init_check: gst_element_factory_make: gst_pipeline_new: gst_bin_add:
gst_element_set_state: gst_element_get_state: gst_object_unref: b dummy_stub

.globl pa_context_new, pa_context_connect, pa_context_disconnect, pa_context_unref
.globl pa_stream_new, pa_stream_connect_playback, pa_stream_connect_record
pa_context_new: pa_context_connect: pa_context_disconnect: pa_context_unref:
pa_stream_new: pa_stream_connect_playback: pa_stream_connect_record: b dummy_stub

.globl libusb_init, libusb_exit, libusb_get_device_list, libusb_free_device_list
.globl libusb_open, libusb_close, libusb_claim_interface, libusb_release_interface
libusb_init: libusb_exit: libusb_get_device_list: libusb_free_device_list:
libusb_open: libusb_close: libusb_claim_interface: libusb_release_interface: b dummy_stub

.globl wl_display_connect, wl_display_disconnect, wl_display_roundtrip
.globl XOpenDisplay, XCloseDisplay, XFlush, XSync, XCreateWindow, XDestroyWindow
wl_display_connect: wl_display_disconnect: wl_display_roundtrip:
XOpenDisplay: XCloseDisplay: XFlush: XSync: XCreateWindow: XDestroyWindow: b dummy_stub
