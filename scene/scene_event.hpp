#ifndef PSYQ_SCENE_EVENT_HPP_
#define PSYQ_SCENE_EVENT_HPP_

//#include <psyq/const_string.hpp>
//#include <psyq/scene/event_action.hpp>

namespace psyq
{
	template< typename, typename, typename > class scene_event;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief scene��event�S�̂��Ǘ�����B
 */
template< typename t_hash, typename t_real, typename t_string >
class psyq::scene_event
{
	typedef psyq::scene_event< t_hash, t_real, t_string > this_type;

	//-------------------------------------------------------------------------
	public: typedef t_hash hash; ///< event-package�Ŏg���Ă���hash�֐��B
	public: typedef t_real real; ///< event-package�Ŏg���Ă�������̌^�B
	public: typedef t_string string; ///< ������̌^�B
	public: typedef psyq::basic_const_string<
		typename t_string::value_type, typename t_string::traits_type >
			const_string; ///< ������萔�̌^�B
	public: typedef psyq::event_item< t_hash > item;
	public: typedef psyq::event_point< t_hash, t_real > point;
	public: typedef psyq::event_line< t_hash, t_real > line;
	public: typedef psyq::event_action< t_hash, t_real, t_string > action;
	public: typedef typename t_string::allocator_type allocator;

	//-------------------------------------------------------------------------
	// event�u����̎����B
	public: typedef std::map<
		typename t_hash::value,
		t_string,
		std::less< typename t_hash::value >,
		typename this_type::allocator::template rebind<
			std::pair< typename t_hash::value const, t_string > >::other >
				word_map;

	// event-line�̎����B
	public: typedef std::map<
		typename t_hash::value,
		typename this_type::line,
		std::less< typename t_hash::value >,
		typename this_type::allocator::template rebind<
			std::pair<
				typename t_hash::value const,
				typename this_type::line > >::other >
					line_map;

	/// event-action�̎����B
	public: typedef std::map<
		typename t_hash::value,
		typename this_type::action::shared_ptr,
		std::less< typename t_hash::value >,
		typename this_type::allocator::template rebind<
			std::pair<
				typename t_hash::value const,
				typename this_type::action::shared_ptr > >::other >
					action_map;

	/// time-scale�̎����B
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
	/** @brief scene-event���\�z�B
	    @param[in] i_package   �g�p����event-package�B
	    @param[in] i_allocator �������Ɏg��memory�����q�B
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
	/** @brief event�S�̂������B
	    @param[in,out] io_target ��������event�S�́B
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
	/** @brief event-action��ǉ��B
	    @param t_action �ǉ�����event-action�̌^�B
	    @return �ǉ�����event-action�ւ̋��Lpointer�B
	 */
	public: template< typename t_action >
	typename this_type::action::shared_ptr const& add_action()
	{
		typename this_type::action::shared_ptr& a_action(
			this->actions_[t_action::get_hash()]);
		a_action = PSYQ_ALLOCATE_SHARED< t_action >(
			this->actions_.get_allocator());
		return a_action;
	}

	/** @brief event�u�����ǉ��B
	    @param[in] i_key  �u�������P���hash�l�B
	    @param[in] i_word �u��������̒P��B
	    @return event�u����B
	 */
	public: t_string const& add_word(
		typename this_type::const_string const& i_key,
		typename this_type::const_string const& i_word)
	{
		t_string& a_word(this->words_[t_hash::generate(i_key)]);
		t_string(i_word.data(), i_word.length()).swap(a_word);
		return a_word;
	}

	/** @brief event-line��ǉ��B
	    @param[in] i_key    event-line�����ɓo�^����key�ƂȂ閼�Ohash�l�B
	    @param[in] i_points ���ɂɂ���event-point�z��̖��Ohash�l�B
		@param[in] i_scale  event-line�ɐݒ肷��time-scale�̖��Ohash�l�B
	    @return �ǉ�����event-line�ւ�pointer�B
	 */
	public: typename this_type::line* add_line(
		typename this_type::line_map::key_type const  i_line,
		typename t_hash::value const                  i_points,
		typename this_type::scale_map::key_type const i_scale = t_hash::EMPTY)
	{
		// ������event-line���������猟���B
		typename this_type::line_map::iterator a_position(
			this->lines_.lower_bound(i_line));
		if (this->lines_.end() == a_position || a_position->first != i_line)
		{
			// �V����event-line�������ɒǉ��B
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
			// ������event-line�̏������Ɏ��s�B
			return NULL;
		}

		// event-line��time-scale��ݒ�B
		if (t_hash::EMPTY != i_scale)
		{
			a_position->second.scale_ = this->get_scale(i_scale);
		}
		return &a_position->second;
	}

	/** @brief time-scale���擾�B
	    @param[in] i_scale time-scale�̖��Ohash�l�B
	    @return time-scale�ւ̋��Lpointer�B
	 */
	typename this_type::line::scale::shared_ptr const& get_scale(
		typename this_type::scale_map::key_type const i_scale)
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

	//-------------------------------------------------------------------------
	/** @brief �u���ꎫ������ď��ɂɑ��݂��镶�����u�����Ahash�l���擾�B
	    @param[in] i_offset �ϊ����镶����̏��ɓ�offset�l�B
	    @return �u����̕������hash�l�B
	 */
	public: typename t_hash::value replace_hash(
		typename this_type::item::offset const i_offset)
	const
	{
		return t_hash::generate(this->replace_string(i_offset));
	}

	/** @brief �u���ꎫ������ĕ������u�����Ahash�l���擾�B
	    @param[in] i_source �u������镶����B
	    @return �u����̕������hash�l�B
	 */
	public: typename t_hash::value replace_hash(
		typename this_type::const_string const& i_source)
	const
	{
		return t_hash::generate(this->replace_string(i_source));
	}

	//-------------------------------------------------------------------------
	/** @brief �u���ꎫ������āA���ɂɑ��݂��镶�����u���B
	    @param[in] i_offset �ϊ����镶�����event-package��offset�l�B
	    @return �u����̕�����B
	 */
	public: t_string replace_string(
		typename this_type::item::offset const i_offset)
	const
	{
		return this->replace_string(this->get_string(i_offset));
	}

	/** @brief �u���ꎫ������āA�������u���B
	    @param[in] i_string �u������镶����B
	    @return �u����̕�����B
	 */
	public: t_string replace_string(
		typename this_type::const_string const& i_source)
	const
	{
		return this_type::item::template replace_word< t_string >(
			this->words_, i_source.begin(), i_source.end());
	}

	//-------------------------------------------------------------------------
	/** @brief ���ɂɑ��݂��镶������擾�B
	    @param[in] i_offset �������event-package��offset�l�B
	 */
	public: typename this_type::const_string get_string(
		typename this_type::item::offset const i_offset)
	const
	{
		// ������̐擪�ʒu���擾�B
		typename t_string::const_pointer const a_begin(
			this->get_address< typename t_string::value_type >(i_offset));
		if (NULL == a_begin)
		{
			return typename this_type::const_string();
		}

		// ���������擾�B
		std::size_t a_length(*a_begin);
		if (a_length <= 0)
		{
			// ��������0�̏ꍇ�́ANULL�����܂Ő�����B
			a_length = t_string::traits_type::length(a_begin + 1);
		}
		return typename this_type::const_string(a_begin + 1, a_length);
	}

	//-------------------------------------------------------------------------
	/** @brief event-package�ɑ��݂���l�ւ�pointer���擾�B
	    @tparam    t_value  �l�̌^�B
	    @param[in] i_offset �l��event-packageoffset�l�B
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
	/** @brief event-package���擾�B
	 */
	public: PSYQ_SHARED_PTR< psyq::event_package const > const& get_package()
	const
	{
		return this->package_;
	}

	//-------------------------------------------------------------------------
	private: PSYQ_SHARED_PTR< psyq::event_package const > package_;

	public: typename this_type::action_map actions_; ///< event-action�̎����B
	public: typename this_type::word_map   words_;   ///< event�u����̎����B
	public: typename this_type::line_map   lines_;   ///< event-line�̎����B
	public: typename this_type::scale_map  scales_;  ///< time-scale�̎����B
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
