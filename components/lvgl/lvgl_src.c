

#if 0
/** core*/
#include ".\src\core\lv_disp.c"
#include ".\src\core\lv_event.c"
#include ".\src\core\lv_group.c"
#include ".\src\core\lv_indev.c"
#include ".\src\core\lv_indev_scroll.c"
#include ".\src\core\lv_obj.c"
#include ".\src\core\lv_obj_class.c"
#include ".\src\core\lv_obj_draw.c"
#include ".\src\core\lv_obj_pos.c"
#include ".\src\core\lv_obj_scroll.c"
#include ".\src\core\lv_obj_style.c"
#include ".\src\core\lv_obj_style_gen.c"
#include ".\src\core\lv_obj_tree.c"
#include ".\src\core\lv_refr.c"
#include ".\src\core\lv_theme.c"

/**draw */
#include ".\src\draw\lv_draw.c"
#include ".\src\draw\lv_draw_arc.c"
#include ".\src\draw\lv_draw_img.c"
#include ".\src\draw\lv_draw_label.c"
#include ".\src\draw\lv_draw_line.c"
#include ".\src\draw\lv_draw_mask.c"
#include ".\src\draw\lv_draw_rect.c"
#include ".\src\draw\lv_draw_triangle.c"
#include ".\src\draw\lv_img_buf.c"
#include ".\src\draw\lv_img_cache.c"
#include ".\src\draw\lv_img_decoder.c"
#include ".\src\draw\sw\lv_draw_sw.c"
#include ".\src\draw\sw\lv_draw_sw_arc.c"
#include ".\src\draw\sw\lv_draw_sw_blend.c"
#include ".\src\draw\sw\lv_draw_sw_img.c"
#include ".\src\draw\sw\lv_draw_sw_letter.c"
#include ".\src\draw\sw\lv_draw_sw_line.c"
#include ".\src\draw\sw\lv_draw_sw_polygon.c"
#include ".\src\draw\sw\lv_draw_sw_rect.c"
#include ".\src\draw\sw\lv_draw_sw_gradient.c"
#include ".\src\draw\sw\lv_draw_sw_dither.c"

/**extra */
#include ".\src\extra\lv_extra.c"
#include ".\src\extra\layouts\flex\lv_flex.c"
#include ".\src\extra\layouts\grid\lv_grid.c"
#include ".\src\extra\themes\basic\lv_theme_basic.c"
#include ".\src\extra\themes\default\lv_theme_default.c"
#include ".\src\extra\themes\mono\lv_theme_mono.c"
#include ".\src\extra\widgets\animimg\lv_animimg.c"
#include ".\src\extra\widgets\calendar\lv_calendar_header_arrow.c"
#include ".\src\extra\widgets\calendar\lv_calendar_header_dropdown.c"
#include ".\src\extra\widgets\chart\lv_chart.c"
#include ".\src\extra\widgets\colorwheel\lv_colorwheel.c"
#include ".\src\extra\widgets\imgbtn\lv_imgbtn.c"
#include ".\src\extra\widgets\keyboard\lv_keyboard.c"
#include ".\src\extra\widgets\led\lv_led.c"
#include ".\src\extra\widgets\list\lv_list.c"
#include ".\src\extra\widgets\menu\lv_menu.c"
#include ".\src\extra\widgets\meter\lv_meter.c"
#include ".\src\extra\widgets\msgbox\lv_msgbox.c"
#include ".\src\extra\widgets\span\lv_span.c"
#include ".\src\extra\widgets\spinbox\lv_spinbox.c"
#include ".\src\extra\widgets\spinner\lv_spinner.c"
#include ".\src\extra\widgets\tabview\lv_tabview.c"
#include ".\src\extra\widgets\tileview\lv_tileview.c"
#include ".\src\extra\widgets\win\lv_win.c"
#include ".\src\extra\widgets\calendar\lv_calendar.c"

/** font */
#include ".\src\font\lv_font.c"
#include ".\src\font\lv_font_dejavu_16_persian_hebrew.c"
#include ".\src\font\lv_font_fmt_txt.c"
#include ".\src\font\lv_font_loader.c"
#include ".\src\font\lv_font_montserrat_8.c"
#include ".\src\font\lv_font_montserrat_10.c"
#include ".\src\font\lv_font_montserrat_12.c"
#include ".\src\font\lv_font_montserrat_14.c"
#include ".\src\font\lv_font_montserrat_16.c"
#include ".\src\font\lv_font_montserrat_18.c"
#include ".\src\font\lv_font_montserrat_20.c"
#include ".\src\font\lv_font_montserrat_22.c"
#include ".\src\font\lv_font_montserrat_24.c"
#include ".\src\font\lv_font_montserrat_26.c"
#include ".\src\font\lv_font_montserrat_28.c"
#include ".\src\font\lv_font_montserrat_30.c"
#include ".\src\font\lv_font_montserrat_32.c"
#include ".\src\font\lv_font_montserrat_34.c"
#include ".\src\font\lv_font_montserrat_36.c"
#include ".\src\font\lv_font_montserrat_38.c"
#include ".\src\font\lv_font_montserrat_40.c"
#include ".\src\font\lv_font_montserrat_42.c"
#include ".\src\font\lv_font_montserrat_44.c"
#include ".\src\font\lv_font_montserrat_46.c"
#include ".\src\font\lv_font_montserrat_48.c"
#include ".\src\font\lv_font_simsun_16_cjk.c"
#include ".\src\font\lv_font_unscii_8.c"
#include ".\src\font\lv_font_unscii_16.c"

/** hal */
#include ".\src\hal\lv_hal_disp.c"
#include ".\src\hal\lv_hal_indev.c"
#include ".\src\hal\lv_hal_tick.c"

/** misc */
#include ".\src\misc\lv_anim.c"
#include ".\src\misc\lv_anim_timeline.c"
#include ".\src\misc\lv_area.c"
#include ".\src\misc\lv_async.c"
#include ".\src\misc\lv_bidi.c"
#include ".\src\misc\lv_color.c"
#include ".\src\misc\lv_fs.c"
#include ".\src\misc\lv_gc.c"
#include ".\src\misc\lv_ll.c"
#include ".\src\misc\lv_log.c"
#include ".\src\misc\lv_lru.c"
#include ".\src\misc\lv_math.c"
#include ".\src\misc\lv_mem.c"
#include ".\src\misc\lv_printf.c"
#include ".\src\misc\lv_style.c"
#include ".\src\misc\lv_style_gen.c"
#include ".\src\misc\lv_templ.c"
#include ".\src\misc\lv_timer.c"
#include ".\src\misc\lv_tlsf.c"
#include ".\src\misc\lv_txt.c"
#include ".\src\misc\lv_txt_ap.c"
#include ".\src\misc\lv_utils.c"

/** widgets */
#include ".\src\widgets\lv_arc.c"
#include ".\src\widgets\lv_bar.c"
#include ".\src\widgets\lv_btn.c"
#include ".\src\widgets\lv_btnmatrix.c"
#include ".\src\widgets\lv_canvas.c"
#include ".\src\widgets\lv_checkbox.c"
#include ".\src\widgets\lv_dropdown.c"
#include ".\src\widgets\lv_img.c"
#include ".\src\widgets\lv_label.c"
#include ".\src\widgets\lv_line.c"
#include ".\src\widgets\lv_objx_templ.c"
#include ".\src\widgets\lv_roller.c"
#include ".\src\widgets\lv_slider.c"
#include ".\src\widgets\lv_switch.c"
#include ".\src\widgets\lv_table.c"
#include ".\src\widgets\lv_textarea.c"

#endif
