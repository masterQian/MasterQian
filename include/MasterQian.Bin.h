#pragma once

#include "MasterQian.Meta.h"

#include <string>
#include <vector>

#define MasterQianModuleVersion 20231229ULL
#undef MasterQianModuleVersion

namespace MasterQian {
	/// <summary>
	/// 字节集视图
	/// </summary>
	struct BinView {
		using ValueT = mqbyte;
		using SizeT = mqui64;
		using PointerT = ValueT const*;

		PointerT mData{ };
		SizeT mSize{ };

		constexpr BinView() noexcept = default;

		/// <param name="data">字节集首址</param>
		/// <param name="size">字节集长度</param>
		BinView(PointerT data, SizeT size) noexcept : mData{ data }, mSize{ size } {}

		[[nodiscard]] bool operator == (BinView bv) const noexcept {
			if (mSize != bv.mSize) return false;
			return std::equal(mData, mData + mSize, bv.mData, bv.mData + bv.mSize);
		}

		[[nodiscard]] ValueT operator [] (SizeT pos) const noexcept {
			return mData[pos];
		}

		/// <summary>
		/// 取长度
		/// </summary>
		[[nodiscard]] SizeT size() const noexcept {
			return mSize;
		}

		/// <summary>
		/// 取32位长度
		/// </summary>
		[[nodiscard]] mqui32 size32() const noexcept {
			return static_cast<mqui32>(mSize);
		}

		/// <summary>
		/// 取首址
		/// </summary>
		[[nodiscard]] PointerT data() const noexcept {
			return mData;
		}

		/// <summary>
		/// 是否为空
		/// </summary>
		[[nodiscard]] bool empty() const noexcept {
			return mSize;
		}

		/// <summary>
		/// 迭代器首
		/// </summary>
		[[nodiscard]] PointerT begin() const noexcept {
			return mData;
		}

		/// <summary>
		/// 迭代器尾
		/// </summary>
		[[nodiscard]] PointerT end() const noexcept {
			return mData + mSize;
		}
	};

	/// <summary>
	/// 字节集
	/// </summary>
	struct Bin : private std::vector<mqbyte> {
		using ValueT = mqbyte;
		using SizeT = mqui64;
		using PointerT = ValueT*;
		using ConstPointerT = ValueT const*;
		using BaseT = std::vector<mqbyte>;
		using StructT = Bin;
		using ReferenceT = Bin&;
		using ConstReferenceT = Bin const&;

		constexpr Bin() noexcept {}

		/// <summary>
		/// 创建指定长度的空字节集
		/// </summary>
		/// <param name="len">长度</param>
		explicit Bin(SizeT len) noexcept : BaseT::vector(len) {}

		/// <summary>
		/// 创建内存首址与长度确定的字节集
		/// </summary>
		/// <param name="data">首址</param>
		/// <param name="len">长度</param>
		Bin(ConstPointerT data, SizeT len) noexcept : BaseT::vector(data, data + len) {}

		/// <summary>
		/// 重复指定字节集一定次数
		/// </summary>
		/// <param name="bin">字节集</param>
		/// <param name="count">重复次数</param>
		Bin(ConstReferenceT bin, SizeT count) noexcept {
			reserve(bin.size() * count);
			for (SizeT i{ }; i < count; ++i) {
				insert(end(), bin.begin(), bin.end());
			}
		}

		/// <summary>
		/// 初始化列表构造字节集
		/// </summary>
		/// <param name="bin">初始化列表</param>
		Bin(std::initializer_list<ValueT> bin) noexcept : BaseT::vector(bin) {}

		/// <summary>
		/// 重复指定初始化列表一定次数
		/// </summary>
		/// <param name="bin">初始化列表</param>
		/// <param name="count">重复次数</param>
		Bin(std::initializer_list<ValueT> bin, SizeT count) noexcept {
			reserve(bin.size() * count);
			for (SizeT i{ }; i < count; ++i) {
				insert(end(), bin.begin(), bin.end());
			}
		}

		/// <summary>
		/// 从字节集视图构造字节集
		/// </summary>
		/// <param name="bv">字节集视图</param>
		explicit Bin(BinView bv) noexcept : BaseT::vector(bv.mData, bv.mData + bv.mSize) {}

		[[nodiscard]] StructT operator + (ConstReferenceT bin) const noexcept {
			StructT tmp;
			tmp.reserve(size() + bin.size());
			tmp.insert(tmp.end(), begin(), end());
			tmp.insert(tmp.end(), bin.begin(), bin.end());
			return tmp;
		}

		ReferenceT operator += (ConstReferenceT bin) noexcept {
			insert(end(), bin.begin(), bin.end());
			return *this;
		}

		[[nodiscard]] bool operator == (ConstReferenceT bin) const noexcept {
			return std::operator==(*this, bin);
		}

		using BaseT::operator[];
		using BaseT::size;
		using BaseT::data;
		using BaseT::empty;
		using BaseT::begin;
		using BaseT::end;
		using BaseT::cbegin;
		using BaseT::cend;
		using BaseT::rbegin;
		using BaseT::rend;
		using BaseT::crbegin;
		using BaseT::crend;
		using BaseT::clear;
		using BaseT::resize;
		
		[[nodiscard]] operator BinView() const noexcept {
			return { data(), size() };
		}

		/// <summary>
		/// 取32位长度
		/// </summary>
		[[nodiscard]] mqui32 size32() const noexcept {
			return static_cast<mqui32>(size());
		}

		/// <summary>
		/// 取字节集左边
		/// </summary>
		/// <param name="num">字节数</param>
		[[nodiscard]] StructT left(SizeT num) const noexcept {
			StructT tmp;
			if (num <= size()) {
				tmp.insert(tmp.end(), begin(), begin() + num);
			}
			return tmp;
		}

		/// <summary>
		/// 取字节集右边
		/// </summary>
		/// <param name="num">字节数</param>
		[[nodiscard]] StructT right(SizeT num) const noexcept {
			StructT tmp;
			if (num <= size()) {
				tmp.insert(tmp.end(), begin() + size() - num, end());
			}
			return tmp;
		}

		/// <summary>
		/// 取字节集中间
		/// </summary>
		/// <param name="start">起始位置</param>
		/// <param name="num">字节数</param>
		[[nodiscard]] StructT middle(SizeT start, SizeT num) const noexcept {
			StructT tmp;
			if (start + num <= size()) {
				tmp.insert(tmp.end(), begin() + start, begin() + start + num);
			}
			return tmp;
		}

		/// <summary>
		/// 导出以大括号包裹显示每个字节内容的友好字符串，对于超出520长度的字节集不作显示
		/// </summary>
		[[nodiscard]] std::wstring show() const noexcept {
			auto len{ size() };
			std::wstring bs = L"Bin[";
			bs += std::to_wstring(len);
			bs += L"]";
			if (len <= 520ULL) { // specific information will not be displayed if the length exceeds 520
				bs.reserve(4 * len); // 0 - 255 has 2 character length in average
				bs.push_back(L'{');
				for (auto v : *this) {
					bs += std::to_wstring(v);
					bs.push_back(L',');
				}
				if (len) bs.back() = L'}';
				else bs.push_back(L'}');
			}
			return bs;
		}
	};
}