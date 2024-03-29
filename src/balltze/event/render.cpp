// SPDX-License-Identifier: GPL-3.0-only

#include <balltze/event.hpp>
#include <balltze/api.hpp>
#include <balltze/memory.hpp>
#include <balltze/hook.hpp>
#include <balltze/command.hpp>
#include "../logger.hpp"

namespace Balltze::Event {
    static std::uint32_t render_ui_param;

    extern "C" {
        bool ui_render_event_before_dispatcher_asm();
    
        bool ui_render_event_before_dispatcher(std::uint32_t unknown) {
            render_ui_param = unknown;
            UIRenderEventArgs args = {unknown};
            UIRenderEvent ui_render_event(EVENT_TIME_BEFORE, args);
            ui_render_event.dispatch();
            return ui_render_event.cancelled();
        }
    
        void ui_render_event_after_dispatcher() {
            UIRenderEventArgs args = {render_ui_param};
            UIRenderEvent ui_render_event(EVENT_TIME_AFTER, args);
            ui_render_event.dispatch();
        }
    }

    template<>
    void EventHandler<UIRenderEvent>::init() {
        static bool enabled = false;
        if(enabled) {
            return;
        }
        enabled = true;

        if(get_balltze_side() != BALLTZE_SIDE_CLIENT) {
            return;
        }

        auto *render_ui_sig = Memory::get_signature("render_ui_function_call");
        if(!render_ui_sig) {
            throw std::runtime_error("Could not find signature for UI render event");
        }

        Memory::hook_function(render_ui_sig->data(), std::function<bool()>(ui_render_event_before_dispatcher_asm), ui_render_event_after_dispatcher);
    }

    static bool hud_render_event_before_dispatcher() {
        HUDRenderEvent hud_render_event(EVENT_TIME_BEFORE);
        hud_render_event.dispatch();
        return hud_render_event.cancelled();
    }

    static void hud_render_event_after_dispatcher() {
        HUDRenderEvent hud_render_event(EVENT_TIME_AFTER);
        hud_render_event.dispatch();
    }

    template<>
    void EventHandler<HUDRenderEvent>::init() {
        static bool enabled = false;
        if(enabled) {
            return;
        }
        enabled = true;

        if(get_balltze_side() != BALLTZE_SIDE_CLIENT) {
            return;
        }

        auto *render_hud_sig = Memory::get_signature("render_hud_function_call");
        if(!render_hud_sig) {
            throw std::runtime_error("Could not find signature for HUD render event");
        }

        Memory::hook_function(render_hud_sig->data(), std::function<bool()>(hud_render_event_before_dispatcher), hud_render_event_after_dispatcher);
    }

    static bool post_carnage_report_render_event_before_dispatcher() {
        PostCarnageReportRenderEvent post_carnage_report_render_event(EVENT_TIME_BEFORE);
        post_carnage_report_render_event.dispatch();
        return post_carnage_report_render_event.cancelled();
    }

    static void post_carnage_report_render_event_after_dispatcher() {
        PostCarnageReportRenderEvent post_carnage_report_render_event(EVENT_TIME_AFTER);
        post_carnage_report_render_event.dispatch();
    }

    template<>
    void EventHandler<PostCarnageReportRenderEvent>::init() {
        static bool enabled = false;
        if(enabled) {
            return;
        }
        enabled = true;

        if(get_balltze_side() != BALLTZE_SIDE_CLIENT) {
            return;
        }

        auto *render_post_carnage_report_call_sig = Memory::get_signature("render_post_carnage_report_call");
        if(!render_post_carnage_report_call_sig) {
            throw std::runtime_error("Could not find signature for post carnage report render event");
        }

        Memory::hook_function(render_post_carnage_report_call_sig->data(), std::function<bool()>(post_carnage_report_render_event_before_dispatcher), post_carnage_report_render_event_after_dispatcher);
    }

    static Engine::TagDefinitions::BitmapData *hud_element_bitmap_render_event_bitmap_data;

    extern "C" {
        bool hud_element_bitmap_render_event_before_dispatcher_asm();
        void hud_element_bitmap_render_event_after_dispatcher_asm();
    
        bool hud_element_bitmap_render_event_before_dispatcher(UIWidgetRenderVertices &vertices, Engine::TagDefinitions::BitmapData *bitmap_data) {
            HUDElementBitmapRenderEventArgs args;
            args.vertices = &vertices;
            args.bitmap_data = bitmap_data;
            hud_element_bitmap_render_event_bitmap_data = bitmap_data;
            HUDElementBitmapRenderEvent widget_background_render_event(EVENT_TIME_BEFORE, args);
            widget_background_render_event.dispatch();
            return widget_background_render_event.cancelled();
        }
    
        void hud_element_bitmap_render_event_after_dispatcher(UIWidgetRenderVertices &vertices) {
            HUDElementBitmapRenderEventArgs args;
            args.vertices = &vertices;
            args.bitmap_data = hud_element_bitmap_render_event_bitmap_data;
            HUDElementBitmapRenderEvent widget_background_render_event(EVENT_TIME_AFTER, args);
            widget_background_render_event.dispatch();
        }
    }

    static bool debug_hud_element_bitmap_render_event(int arg_count, const char **args) {
        static std::optional<Event::EventListenerHandle<HUDElementBitmapRenderEvent>> handle;
        if(arg_count == 1) {
            bool new_setting = STR_TO_BOOL(args[0]);
            if(new_setting) {
                if(handle) {
                    handle->remove();
                    handle = std::nullopt;
                }
                handle = Event::HUDElementBitmapRenderEvent::subscribe([](HUDElementBitmapRenderEvent &event) {
                    auto &arguments = event.args;
                    auto time = event_time_to_string(event.time);
                    logger.debug("HUD element bitmap render event ({}): bitmap data: {}, vertices: {}", time, reinterpret_cast<std::uint32_t>(arguments.bitmap_data), reinterpret_cast<std::uint32_t>(&arguments.vertices));
                });
            }
            else {
                if(handle) {
                    handle->remove();
                    handle = std::nullopt;
                }
            }
        }
        logger.info("debug_hud_element_bitmap_render_event: {}", handle.has_value());
        return true;
    }

    static EventListenerHandle<TickEvent> hud_element_bitmap_render_event_init_tick_event_handle;

    template<>
    void EventHandler<HUDElementBitmapRenderEvent>::init() {
        static bool enabled = false;
        if(enabled) {
            return;
        }
        enabled = true;

        if(get_balltze_side() != BALLTZE_SIDE_CLIENT) {
            return;
        }

        hud_element_bitmap_render_event_init_tick_event_handle = TickEvent::subscribe([](TickEvent const &event) {
            auto *render_hud_element_bitmap_function_call_sig = Memory::get_signature("render_hud_element_bitmap_function_call");
            if(!render_hud_element_bitmap_function_call_sig) {
                throw std::runtime_error("Could not find signature for HUD element bitmap render event");
            }
            auto *address = Memory::follow_32bit_jump(render_hud_element_bitmap_function_call_sig->data()) + 5;
            auto before_dispatcher = std::function<bool()>(hud_element_bitmap_render_event_before_dispatcher_asm);
            Memory::hook_function(address, before_dispatcher, hud_element_bitmap_render_event_after_dispatcher_asm);

            hud_element_bitmap_render_event_init_tick_event_handle.remove();
        });

        // Register debug command
        register_command("debug_hud_element_bitmap_render_event", "debug", "Sets whenever to log HUD element bitmap render event.", "[enable: boolean]", debug_hud_element_bitmap_render_event, true, 0, 1);
    }

    extern "C" {
        bool widget_background_render_event_before_dispatcher_asm();
        void widget_background_render_event_after_dispatcher_asm();
    
        bool widget_background_render_event_before_dispatcher(UIWidgetRenderVertices &vertices, Engine::Widget *widget) {
            UIWidgetBackgroundRenderEventArgs args;
            args.vertices = &vertices;
            args.widget = widget;
            UIWidgetBackgroundRenderEvent widget_background_render_event(EVENT_TIME_BEFORE, args);
            widget_background_render_event.dispatch();
            return widget_background_render_event.cancelled();
        }
    
        void widget_background_render_event_after_dispatcher(UIWidgetRenderVertices &vertices, Engine::Widget *widget) {
            UIWidgetBackgroundRenderEventArgs args;
            args.vertices = &vertices;
            args.widget = widget;
            UIWidgetBackgroundRenderEvent widget_background_render_event(EVENT_TIME_AFTER, args);
            widget_background_render_event.dispatch();
        }
    }

    static bool debug_widget_background_render_event(int arg_count, const char **args) {
        static std::optional<Event::EventListenerHandle<UIWidgetBackgroundRenderEvent>> handle;
        if(arg_count == 1) {
            bool new_setting = STR_TO_BOOL(args[0]);
            if(new_setting) {
                if(handle) {
                    handle->remove();
                    handle = std::nullopt;
                }
                handle = Event::UIWidgetBackgroundRenderEvent::subscribe_const([](UIWidgetBackgroundRenderEvent const &event) {
                    auto &arguments = event.args;
                    auto time = event_time_to_string(event.time);
                    logger.debug("Widget background render event ({}): widget: {}", time, reinterpret_cast<std::uint32_t>(arguments.widget));
                });
            }
            else {
                if(handle) {
                    handle->remove();
                    handle = std::nullopt;
                }
            }
        }
        logger.info("debug_widget_background_render_event: {}", handle.has_value());
        return true;
    }

    static EventListenerHandle<TickEvent> widget_background_render_event_init_tick_event_handle;

    template<>
    void EventHandler<UIWidgetBackgroundRenderEvent>::init() {
        static bool enabled = false;
        if(enabled) {
            return;
        }
        enabled = true;

        if(get_balltze_side() != BALLTZE_SIDE_CLIENT) {
            return;
        }

        widget_background_render_event_init_tick_event_handle = TickEvent::subscribe([](TickEvent const &event) {
            auto *render_widget_background_function_call_sig = Memory::get_signature("render_widget_background_function_call");
            if(!render_widget_background_function_call_sig) {
                throw std::runtime_error("Could not find signature for widget background render event");
            }
            // auto *address = Memory::follow_32bit_jump(render_widget_background_function_call_sig->data()) + 5;
            auto before_dispatcher = std::function<bool()>(widget_background_render_event_before_dispatcher_asm);
            Memory::hook_function(render_widget_background_function_call_sig->data(), before_dispatcher, widget_background_render_event_after_dispatcher_asm);

            widget_background_render_event_init_tick_event_handle.remove();
        });

        // Register debug command
        register_command("debug_widget_background_render_event", "debug", "Sets whenever to log widget background render event.", "[enable: boolean]", debug_widget_background_render_event, true, 0, 1);
    }

    static bool navpoints_render_event_before_dispatcher() {
        NavPointsRenderEvent navpoints_render_event(EVENT_TIME_BEFORE);
        navpoints_render_event.dispatch();
        return navpoints_render_event.cancelled();
    }

    static void navpoints_render_event_after_dispatcher() {
        NavPointsRenderEvent navpoints_render_event(EVENT_TIME_AFTER);
        navpoints_render_event.dispatch();
    }

    static EventListenerHandle<TickEvent> navpoints_render_event_init_tick_event_handle;

    template<>
    void EventHandler<NavPointsRenderEvent>::init() {
        static bool enabled = false;
        if(enabled) {
            return;
        }
        enabled = true;

        if(get_balltze_side() != BALLTZE_SIDE_CLIENT) {
            return;
        }

        navpoints_render_event_init_tick_event_handle = TickEvent::subscribe([](TickEvent const &event) {
            auto *render_navpoint_function_call_sig = Memory::get_signature("render_navpoint_function_call");
            if(!render_navpoint_function_call_sig) {
                throw std::runtime_error("Could not find signature for navpoints render event");
            }

            auto *address = Memory::follow_32bit_jump(render_navpoint_function_call_sig->data()) + 9;
            Memory::hook_function(address, std::function<bool()>(navpoints_render_event_before_dispatcher), navpoints_render_event_after_dispatcher);

            navpoints_render_event_init_tick_event_handle.remove();
        });
    }
}
