#pragma once

#include "render/render_base.hh"

namespace nrender
{
    class UI_Context: public RenderContext
    {
        public:
            bool init(nwindow::IWindow* window) override;

            void pre_render() override;

            void post_render() override;

            void end() override;
    }
} // namespace nrender
