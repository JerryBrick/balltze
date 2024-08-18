

#include <balltze/engine/data_types.hpp>
#include <balltze/memory.hpp>
#include <balltze/hook.hpp>

namespace Balltze::Features {
    extern "C" {
        void hud_meters_draw_hook();

        void apply_hud_meter_translucency(Engine::ColorARGBInt *mask) {

        }
    }

    void set_up_hud_meters_fix() {
        // Hook the hud_meters_draw function
        Memory::hook_function((void *)0x4af487, hud_meters_draw_hook, {}, false);
    }
}
