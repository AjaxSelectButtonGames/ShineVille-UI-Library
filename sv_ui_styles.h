#pragma once
#include "sv_ui2.0.h"

namespace SV_UI {

	struct ButtonStyle {
		float normalColor[3];
		float hoverColor[3];
		float pressedColor[3];
	};

    struct Styles {
        ButtonStyle buttonStyle;

        // Define default styles in the constructor
        Styles() {
            // Default button colors
            buttonStyle.normalColor[0] = 0.7f; buttonStyle.normalColor[1] = 0.7f; buttonStyle.normalColor[2] = 0.7f;
            buttonStyle.hoverColor[0] = 0.8f; buttonStyle.hoverColor[1] = 0.8f; buttonStyle.hoverColor[2] = 0.8f;
            buttonStyle.pressedColor[0] = 0.6f; buttonStyle.pressedColor[1] = 0.6f; buttonStyle.pressedColor[2] = 0.6f;
        }

        // Add methods to set custom styles if needed
        void setButtonStyle(const ButtonStyle& style) {
            buttonStyle = style;
        }
    };

    //Global styles instance
   Styles styles;


}