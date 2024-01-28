#pragma once

#include "MasterQian.Meta.h"

#define MasterQianModuleVersion 20240103ULL
#undef MasterQianModuleVersion

namespace MasterQian::Media {
	/// <summary>
	/// ARGB��ɫ
	/// </summary>
	struct Color {
	private:
		mqi32 color;
	public:
		constexpr Color(mqbyte r, mqbyte g, mqbyte b, mqbyte alpha = 255) noexcept {
			color = static_cast<mqi32>(b) | (static_cast<mqi32>(g) << 8) |
				(static_cast<mqi32>(r) << 16) | (static_cast<mqi32>(alpha) << 24);
		}

		explicit constexpr Color(mqui32 argb) noexcept : color{ static_cast<mqi32>(argb) } {}

		constexpr mqbyte A() const noexcept {
			return static_cast<mqbyte>(color >> 24) & 0xFF;
		}

		constexpr mqbyte R() const noexcept {
			return static_cast<mqbyte>(color >> 16) & 0xFF;
		}

		constexpr mqbyte G() const noexcept {
			return static_cast<mqbyte>(color >> 8) & 0xFF;
		}

		constexpr mqbyte B() const noexcept {
			return static_cast<mqbyte>(color) & 0xFF;
		}

		constexpr operator mqui32() const noexcept {
			return color;
		}

	};

	/// <summary>
	/// ��ɫö��
	/// </summary>
	struct Colors {
		Colors() = delete;
#define COLOR_ENUM(name, R, G, B, A, comment) static constexpr Color name{ R, G, B, A }
		COLOR_ENUM(Transparency, 0, 0, 0, 0, "͸��");
		COLOR_ENUM(White, 255, 255, 255, 255, "��ɫ");
		COLOR_ENUM(Black, 0, 0, 0, 255, "��ɫ");
		COLOR_ENUM(Red, 255, 0, 0, 255, "��ɫ");
		COLOR_ENUM(Orange, 255, 165, 0, 255, "��ɫ");
		COLOR_ENUM(Yellow, 255, 255, 0, 255, "��ɫ");
		COLOR_ENUM(Green, 0, 255, 0, 255, "��ɫ");
		COLOR_ENUM(Cyan, 0, 255, 255, 255, "��ɫ");
		COLOR_ENUM(Blue, 0, 0, 255, 255, "��ɫ");
		COLOR_ENUM(Purple, 128, 0, 128, 255, "��ɫ");
		COLOR_ENUM(Gray, 128, 128, 128, 255, "��ɫ");
		COLOR_ENUM(Brown, 165, 42, 42, 255, "��ɫ");
		COLOR_ENUM(Pink, 255, 192, 203, 255, "��ɫ");
#undef COLOR_ENUM
	};
}