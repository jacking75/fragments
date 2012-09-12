#ifndef PSYQ_SCENE_EVENT_HPP_
#define PSYQ_SCENE_EVENT_HPP_

//#include <psyq/const_string.hpp>
//#include <psyq/scene/event_action.hpp>

namespace psyq
{
	template< typename, typename, typename > class scene_event;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief sceneのevent全体を管理する。
    @tparam t_hash   event-packageで使われているhash関数。
    @tparam t_real   event-packageで使われている実数の型。
    @tparam t_string std::basic_string互換の文字列の型。
 */
template< typename t_hash, typename t_real, typename t_string >
class psyq::scene_event
{
	typedef psyq::scene_event< t_hash, t_real, t_string > this_type;

	//-------------------------------------------------------------------------
	public: typedef t_hash hash;
	public: typedef t_real real;
	public: typedef t_string string;
	public: typedef psyq::basic_const_string<
		typename t_string::value_type, typename t_string::traits_type >
			const_string; ///< 文字列定数の型。
	public: typedef psyq::event_item< t_hash > item;
	public: typedef psyq::event_point< t_hash, t_real > point;
	public: typedef psyq::event_line< t_hash, t_real > line;
	public: typedef psyq::event_action< t_hash, t_real, t_string > action;
	public: typedef typename t_string::allocator_type allocator;

	//-------------------------------------------------------------------------
	// event置換語の辞書。
	public: typedef std::map<
		typename t_hash::value,
		t_string,
		std::less< typename t_hash::value >,
		typename this_type::allocator::template rebind<
			std::pair< typename t_hash::value const, t_string > >::other >
				word_map;

	// event-lineの辞書。
	public: typedef std::map<
		typename t_hash::value,
		typename this_type::line,
		std::less< typename t_hash::value >,
		typename this_type::allocator::template rebind<
			std::pair<
				typename t_hash::value const,
				typename this_type::line > >::other >
					line_map;

	/// event-actionの辞書。
	public: typedef std::map<
		typename t_hash::value,
		typename this_type::action::shared_ptr,
		std::less< typename t_hash::value >,
		typename this_type::allocator::template rebind<
			std::pair<
				typename t_hash::value const,
				typename this_type::action::shared_ptr > >::other >
					action_map;

	/// time-scaleの辞書。
	public: typedef std::map<
		typename t_hash::value,
		typename this_type::line::scale::shared_ptr,
		std::less< typename t_hash::value >,
		typename this_type::allocator::template rebind<
			std::pair<
				typename t_hash::value const,
				typename this_type::line::scale::shared_ptr > >::other >
					scale_map;

	//-------------------------------------------------------------------------
	/** @brief scene-eventを構築。
	    @param[in] i_package   使用するevent-package。
	    @param[in] i_allocator 初期化に使うmemory割当子。
	 */
	public: template< typename t_other_allocator >
	scene_event(
		PSYQ_SHARED_PTR< psyq::event_package const > const& i_package,
		t_other_allocator const&                            i_allocator):
	package_(i_package),
	actions_(typename this_type::action_map::key_compare(), i_allocator),
	words_(typename this_type::word_map::key_compare(), i_allocator),
	lines_(typename this_type::line_map::key_compare(), i_allocator),
	scales_(typename this_type::scale_map::key_compare(), i_allocator)
	{
		// pass
	}

	//-------------------------------------------------------------------------
	/** @brief event全体を交換。
	    @param[in,out] io_target 交換するevent全体。
	 */
	public: void swap(this_type& io_target)
	{
		this->package_.swap(io_target.package_);
		this->actions_.swap(io_target.actions_);
		this->words_.swap(io_target.words_);
		this->lines_.swap(io_target.lines_);
		this->scales_.swap(io_target.scales_);
	}

	//-------------------------------------------------------------------------
	/** @brief event-actionを初期化。
	    @param t_action 初期化するevent-actionの型。
	    @return 追加したevent-actionへの共有pointer。
	 */
	public: template< typename t_action >
	typename this_type::action::shared_ptr const& reset_action()
	{
		typename this_type::action::shared_ptr& a_action(
			this->actions_[t_action::get_hash()]);
		a_action = PSYQ_ALLOCATE_SHARED< t_action >(
			this->actions_.get_allocator());
		return a_action;
	}

	/** @brief event-actionを検索。
	    @tparam t_action 検索するevent-actionの型。
	    @return 見つけたevent-action。見つからなかった場合は空。
	 */
	public: template< typename t_action >
	typename this_type::action::shared_ptr const& find_action() const
	{
		return this_type::_find_element(this->actions_, t_action::get_hash());
	}

	/** @brief event-actionを削除。
	    @tparam t_action 検索するevent-actionの型。
	    @return 削除したevent-action。削除しなかった場合は空。
	 */
	public: template< typename t_action >
	typename this_type::action::shared_ptr const& erase_action() const
	{
		return this_type::_erase_element(this->actions_, t_action::get_hash());
	}

	//-------------------------------------------------------------------------
	/** @brief event-lineを初期化。
	    @param[in] i_line   event-line辞書に登録するkeyとなる名前hash値。
	    @param[in] i_points event-packageにあるevent-point配列の名前hash値。
	    @return 初期化したevent-lineへのpointer。
	 */
	public: typename this_type::line* reset_line(
		typename t_hash::value const i_line,
		typename t_hash::value const i_points)
	{
		// 既存のevent-lineを辞書から検索。
		typename this_type::line_map::iterator a_position(
			this->lines_.lower_bound(i_line));
		if (this->lines_.end() == a_position || a_position->first != i_line)
		{
			// 新たなevent-lineを辞書に追加。
			typename this_type::line a_line(this->package_, i_points);
			if (a_line.is_stop())
			{
				return NULL;
			}
			a_position = this->lines_.insert(
				a_position,
				typename this_type::line_map::value_type(i_line, a_line));
		}
		else if (!a_position->second.reset(this->package_, i_points))
		{
			// 既存のevent-lineの初期化に失敗。
			return NULL;
		}
		return &a_position->second;
	}

	/** @brief event-lineを検索。
	    @param[in] i_line 検索するevent-lineの名前hash値。
	    @return 見つけたevent-lineへのpointer。見つからなかった場合はNULL。
	 */
	public: typename this_type::line* find_line(
		typename t_hash::value const i_line)
	const
	{
		typename this_type::line_map::const_iterator const a_position(
			this->lines_.find(i_line));
		return this->lines_.end() != a_position?
			const_cast< typename this_type::line* >(&a_position->second):
			NULL;
	}

	//-------------------------------------------------------------------------
	/** @brief time-scaleを取得。
	    @param[in] i_scale time-scaleの名前hash値。
	    @return time-scaleへの共有pointer。
	 */
	public: typename this_type::line::scale::shared_ptr const& get_scale(
		typename t_hash::value const i_scale)
	{
		if (t_hash::EMPTY == i_scale)
		{
			return psyq::_get_null_shared_ptr<
				typename this_type::line::scale >();
		}

		typename this_type::line::scale::shared_ptr& a_scale(
			this->scales_[i_scale]);
		if (NULL == a_scale.get())
		{
			PSYQ_ALLOCATE_SHARED< typename this_type::line::scale >(
				this->scales_.get_allocator()).swap(a_scale);
		}
		return a_scale;
	}

	/** @brief time-scaleを検索。
	    @param[in] i_name 検索するtime-scaleの名前hash値。
	    @return 見つけたtime-scale。見つからなかった場合は空。
	 */
	public: typename this_type::line::scale::shared_ptr const& find_scale(
		typename t_hash::value const i_scale)
	const
	{
		return this_type::_find_element(this->scales_, i_scale);
	}

	/** @brief time-scaleを削除。
	    @param[in] i_scale 削除するtime-scaleの名前hash値。
	    @return 削除したtime-scale。削除しなかった場合は空。
	 */
	public: typename this_type::line::scale::shared_ptr const& erase_scale(
		typename t_hash::value const i_scale)
	{
		return this_type::_erase_element(this->scales_, i_scale);
	}

	//-------------------------------------------------------------------------
	/** @brief event置換語を登録。
	    @param[in] i_key  置換される単語のhash値。
	    @param[in] i_word 置換した後の単語。
	    @return 置換した後の単語。
	 */
	public: t_string const& replace_word(
		typename this_type::const_string const& i_key,
		typename this_type::const_string const& i_word)
	{
		t_string& a_word(this->words_[t_hash::generate(i_key)]);
		t_string(i_word.data(), i_word.length()).swap(a_word);
		return a_word;
	}

	//-------------------------------------------------------------------------
	/** @brief 置換語辞書を介してevent-packageに存在する文字列を置換し、hash値を取得。
	    @param[in] i_offset 変換する文字列のevent-package内offset値。
	    @return 置換後の文字列のhash値。
	 */
	public: typename t_hash::value replace_hash(
		typename this_type::item::offset const i_offset)
	const
	{
		return t_hash::generate(this->replace_string(i_offset));
	}

	/** @brief 置換語辞書を介して文字列を置換し、hash値を取得。
	    @param[in] i_source 置換される文字列。
	    @return 置換後の文字列のhash値。
	 */
	public: typename t_hash::value replace_hash(
		typename this_type::const_string const& i_source)
	const
	{
		return t_hash::generate(this->replace_string(i_source));
	}

	//-------------------------------------------------------------------------
	/** @brief 置換語辞書を介して、event-packageに存在する文字列を置換。
	    @param[in] i_offset 置換元となる文字列のevent-package内offset値。
	    @return 置換後の文字列。
	 */
	public: t_string replace_string(
		typename this_type::item::offset const i_offset)
	const
	{
		return this->replace_string(this->get_string(i_offset));
	}

	/** @brief 置換語辞書を介して、文字列を置換。
	    @param[in] i_string 置換元となる文字列。
	    @return 置換後の文字列。
	 */
	public: t_string replace_string(
		typename this_type::const_string const& i_source)
	const
	{
		return this_type::item::template replace_string< t_string >(
			this->words_, i_source.begin(), i_source.end());
	}

	//-------------------------------------------------------------------------
	/** @brief event-packageに存在する文字列を取得。
	    @param[in] i_offset 文字列のevent-package内offset値。
	 */
	public: typename this_type::const_string get_string(
		typename this_type::item::offset const i_offset)
	const
	{
		// 文字列の先頭位置を取得。
		typename t_string::const_pointer const a_begin(
			this->get_address< typename t_string::value_type >(i_offset));
		if (NULL == a_begin)
		{
			return typename this_type::const_string();
		}

		// 文字数を取得。
		std::size_t a_length(*a_begin);
		if (a_length <= 0)
		{
			// 文字数が0の場合は、NULL文字まで数える。
			a_length = t_string::traits_type::length(a_begin + 1);
		}
		return typename this_type::const_string(a_begin + 1, a_length);
	}

	//-------------------------------------------------------------------------
	/** @brief event-packageに存在する値へのpointerを取得。
	    @tparam    t_value  値の型。
	    @param[in] i_offset 値のevent-packageoffset値。
	 */
	public: template< typename t_value >
	t_value const* get_address(
		typename this_type::item::offset const i_offset) const
	{
		psyq::event_package const* const a_package(this->package_.get());
		return NULL != a_package?
			this_type::item::template get_address< t_value >(
				*a_package, i_offset):
			NULL;
	}

	//-------------------------------------------------------------------------
	/** @brief event-packageを取得。
	 */
	public: PSYQ_SHARED_PTR< psyq::event_package const > const& get_package()
	const
	{
		return this->package_;
	}

	//-------------------------------------------------------------------------
	/** @brief containerから要素を検索。
	    @param[in] i_container 対象となるcontainer。
	    @param[in] i_name      削除する要素の名前hash値。
	 */
	public: template< typename t_container >
	static typename t_container::mapped_type const& _find_element(
		t_container const&           i_container,
		typename t_hash::value const i_name)
	{
		typename t_container::const_iterator const a_position(
			i_container.find(i_name));
		return i_container.end() != a_position?
			a_position->second:
			psyq::_get_null_shared_ptr<
				typename t_container::mapped_type::element_type >();
	}

	/** @brief containerから要素を削除。
	    @param[in] i_container 対象となるcontainer。
	    @param[in] i_name      削除する要素の名前hash値。
	 */
	public: template< typename t_container >
	static typename t_container::mapped_type _erase_element(
		t_container&                 io_container,
		typename t_hash::value const i_name)
	{
		typename t_container::mapped_type a_element;
		typename t_container::iterator const a_position(
			io_container.find(i_name));
		if (io_container.end() != a_position)
		{
			a_element.swap(a_position->second);
			io_container.erase(a_position);
		}
		return a_element;
	}

	//-------------------------------------------------------------------------
	private: PSYQ_SHARED_PTR< psyq::event_package const > package_;

	public: typename this_type::action_map actions_; ///< event-actionの辞書。
	public: typename this_type::word_map   words_;   ///< event置換語の辞書。
	public: typename this_type::line_map   lines_;   ///< event-lineの辞書。
	public: typename this_type::scale_map  scales_;  ///< time-scaleの辞書。
};

//-----------------------------------------------------------------------------
namespace std
{
	template< typename t_hash, typename t_real, typename t_string >
	void swap(
		psyq::scene_event< t_hash, t_real, t_string >& io_left,
		psyq::scene_event< t_hash, t_real, t_string >& io_right)
	{
		io_left.swap(io_right);
	}
};

#endif // !PSYQ_SCENE_EVENT_HPP_
