#ifndef PSYQ_SCENE_EVENT_STAGE_HPP_
#define PSYQ_SCENE_EVENT_STAGE_HPP_

//#include <psyq/const_string.hpp>
//#include <psyq/scene/event_action.hpp>

namespace psyq
{
	template< typename, typename, typename, typename > class event_stage;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief event�Ŏg��object��z�u�����B
    @tparam t_hash      event-package�Ŏg���Ă���hash�֐��B
    @tparam t_real      event-package�Ŏg���Ă�������̌^�B
    @tparam t_string    event�u����Ɏg��������̌^�Bstd::basic_string�݊��B
    @tparam t_allocator �g�p����memory�����q�̌^�B
 */
template<
	typename t_hash,
	typename t_real,
	typename t_string,
	typename t_allocator >
class psyq::event_stage
{
	public: typedef psyq::event_stage< t_hash, t_real, t_string, t_allocator >
		this_type;

	//-------------------------------------------------------------------------
	public: typedef t_hash hash;
	public: typedef t_real real;
	public: typedef t_string string;
	public: typedef t_allocator allocator;
	public: typedef psyq::event_item< t_hash > item;
	public: typedef psyq::event_point< t_hash, t_real > point;
	public: typedef psyq::event_line< t_hash, t_real > line;
	public: typedef psyq::event_action< t_hash, t_real > action;
	public: typedef psyq::basic_const_string<
		typename t_string::value_type, typename t_string::traits_type >
			const_string; ///< ������萔�̌^�B

	//-------------------------------------------------------------------------
	/// event�u����̎����B
	public: typedef std::map<
		typename t_hash::value,
		t_string,
		std::less< typename t_hash::value >,
		typename t_allocator::template rebind<
			std::pair< typename t_hash::value const, t_string > >::other >
				word_map;

	/// event-line�̎����B
	public: typedef std::map<
		typename t_hash::value,
		typename this_type::line,
		std::less< typename t_hash::value >,
		typename t_allocator::template rebind<
			std::pair<
				typename t_hash::value const,
				typename this_type::line > >::other >
					line_map;

	/// event-action�̎����B
	public: typedef std::map<
		typename t_hash::value,
		typename this_type::action::shared_ptr,
		std::less< typename t_hash::value >,
		typename t_allocator::template rebind<
			std::pair<
				typename t_hash::value const,
				typename this_type::action::shared_ptr > >::other >
					action_map;

	/// time-scale�̎����B
	public: typedef std::map<
		typename t_hash::value,
		typename this_type::line::scale::shared_ptr,
		std::less< typename t_hash::value >,
		typename t_allocator::template rebind<
			std::pair<
				typename t_hash::value const,
				typename this_type::line::scale::shared_ptr > >::other >
					scale_map;

	//-------------------------------------------------------------------------
	/** @brief event�o�L����\�z�B
	    @param[in] i_package   event�o�L�낪�g��event-package�B
	    @param[in] i_allocator �������Ɏg��memory�����q�B
	 */
	public: event_stage(
		PSYQ_SHARED_PTR< psyq::event_package const > const& i_package,
		t_allocator const&                                  i_allocator):
	package_(i_package),
	actions_(typename this_type::action_map::key_compare(), i_allocator),
	words_(typename this_type::word_map::key_compare(), i_allocator),
	lines_(typename this_type::line_map::key_compare(), i_allocator),
	scales_(typename this_type::scale_map::key_compare(), i_allocator)
	{
		// pass
	}

	//-------------------------------------------------------------------------
	/** @brief event�o�L��������B
	    @param[in,out] io_target ��������event�o�L��B
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
	/** @brief event-action���������B
	    @param t_action ����������event-action�̌^�B
	    @return �ǉ�����event-action�ւ̋��Lpointer�B
	 */
	public: template< typename t_action >
	typename this_type::action::shared_ptr const& make_action()
	{
		typename this_type::action::shared_ptr& a_action(
			this->actions_[t_action::get_hash()]);
		a_action = PSYQ_ALLOCATE_SHARED< t_action >(
			this->actions_.get_allocator());
		return a_action;
	}

	public: template< typename t_action, typename t_param0 >
	typename this_type::action::shared_ptr const& make_action(
		t_param0 const& i_param0)
	{
		typename this_type::action::shared_ptr& a_action(
			this->actions_[t_action::get_hash()]);
		a_action = PSYQ_ALLOCATE_SHARED< t_action >(
			this->actions_.get_allocator(), i_param0);
		return a_action;
	}

	public: template< typename t_action, typename t_param0, typename t_param1 >
	typename this_type::action::shared_ptr const& make_action(
		t_param0 const& i_param0,
		t_param1 const& i_param1)
	{
		typename this_type::action::shared_ptr& a_action(
			this->actions_[t_action::get_hash()]);
		a_action = PSYQ_ALLOCATE_SHARED< t_action >(
			this->actions_.get_allocator(), i_param0, i_param1);
		return a_action;
	}

	/** @brief event-action�������B
	    @tparam t_action ��������event-action�̌^�B
	    @return ������event-action�B������Ȃ������ꍇ�͋�B
	 */
	public: template< typename t_action >
	typename this_type::action::shared_ptr const& find_action() const
	{
		return this_type::_find_element(this->actions_, t_action::get_hash());
	}

	/** @brief event-action����菜���B
	    @tparam t_action ��������event-action�̌^�B
	    @return ��菜����event-action�B��菜���Ȃ������ꍇ�͋�B
	 */
	public: template< typename t_action >
	typename this_type::action::shared_ptr const remove_action() const
	{
		return this_type::_remove_element(
			this->actions_, t_action::get_hash());
	}

	//-------------------------------------------------------------------------
	/** @brief event-line���擾�B
	    ���O�ɑΉ�����event-line�����݂��Ȃ��ꍇ�́A�V���ɍ��B
	    @param[in] i_line �擾����event-line�̖��Ohash�l�B
	    @retval !=NULL event-line�ւ�pointer�B
	    @retval ==NULL ���s�B
	 */
	public: typename this_type::line* get_line(
		typename t_hash::value const i_line)
	{
		return t_hash::EMPTY != i_line? &this->lines_[i_line]: NULL;
	}

	/** @brief event-line�������B
	    @param[in] i_line ��������event-line�̖��Ohash�l�B
	    @retval !=NULL �Y������event-line�ւ�pointer�B
	    @retval ==NULL �Y������event-line��������Ȃ������B
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
	/** @brief time-scale���擾�B
	    ���O�ɑΉ�����time-scale�����݂��Ȃ��ꍇ�́A�V���ɍ��B
	    @param[in] i_scale time-scale�̖��Ohash�l�B
	    @return time-scale�ւ̋��Lpointer�B���s�����ꍇ�͋�B
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

	/** @brief time-scale�������B
	    @param[in] i_name ��������time-scale�̖��Ohash�l�B
	    @return ������time-scale�B������Ȃ������ꍇ�͋�B
	 */
	public: typename this_type::line::scale::shared_ptr const& find_scale(
		typename t_hash::value const i_scale)
	const
	{
		return this_type::_find_element(this->scales_, i_scale);
	}

	/** @brief time-scale����菜���B
	    @param[in] i_scale ��菜��time-scale�̖��Ohash�l�B
	    @return ��菜����time-scale�B��菜���Ȃ������ꍇ�͋�B
	 */
	public: typename this_type::line::scale::shared_ptr const remove_scale(
		typename t_hash::value const i_scale)
	{
		typename this_type::line::scale::shared_ptr const a_scale(
			this_type::_remove_element(this->scales_, i_scale));
		if (NULL != a_scale.get())
		{
			typename this_type::line_map::const_iterator const a_end(
				this->lines_.end());
			for (
				typename this_type::line_map::const_iterator i =
					this->lines_.begin();
				i != a_end;
				++i)
			{
				typename this_type::line& a_line(
					const_cast< typename this_type::line& >(i->second));
				if (a_scale == a_line.scale_)
				{
					a_line.scale_.reset();
				}
			}
		}
		return a_scale;
	}

	//-------------------------------------------------------------------------
	/** @brief event�u�����o�^�B
	    @param[in] i_key  �u�������P���hash�l�B
	    @param[in] i_word �u��������̒P��B
	    @return �u��������̒P��B
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
	/** @brief �u���ꎫ�������event-package�ɑ��݂��镶�����u�����Ahash�l���擾�B
	    @param[in] i_offset �ϊ����镶�����event-package��offset�l�B
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
	/** @brief �u���ꎫ������āAevent-package�ɑ��݂��镶�����u���B
	    @param[in] i_offset �u�����ƂȂ镶�����event-package��offset�l�B
	    @return �u����̕�����B
	 */
	public: t_string replace_string(
		typename this_type::item::offset const i_offset)
	const
	{
		return this->replace_string(this->get_string(i_offset));
	}

	/** @brief �u���ꎫ������āA�������u���B
	    @param[in] i_string �u�����ƂȂ镶����B
	    @return �u����̕�����B
	 */
	public: t_string replace_string(
		typename this_type::const_string const& i_source)
	const
	{
		return this_type::item::template replace_string< t_string >(
			this->words_, i_source.begin(), i_source.end());
	}

	//-------------------------------------------------------------------------
	/** @brief event-package�ɑ��݂��镶������擾�B
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
	/** @brief container����v�f�������B
	    @param[in] i_container �ΏۂƂȂ�container�B
	    @param[in] i_key       ��������v�f��key�B
	    @return ���������v�f�����l�B
	 */
	public: template< typename t_container >
	static typename t_container::mapped_type const& _find_element(
		t_container const&           i_container,
		typename t_hash::value const i_key)
	{
		if (t_hash::EMPTY != i_key)
		{
			typename t_container::const_iterator const a_position(
				i_container.find(i_key));
			if (i_container.end() != a_position)
			{
				return a_position->second;
			}
		}
		return psyq::_get_null_shared_ptr<
			typename t_container::mapped_type::element_type >();
	}

	/** @brief container����v�f����菜���B
	    @param[in] i_container �ΏۂƂȂ�container�B
	    @param[in] i_key       ��菜���v�f��key�B
	    @return ��菜�����v�f�������Ă����l�B
	 */
	public: template< typename t_container >
	static typename t_container::mapped_type _remove_element(
		t_container&                 io_container,
		typename t_hash::value const i_key)
	{
		typename t_container::mapped_type a_element;
		if (t_hash::EMPTY != i_key)
		{
			typename t_container::iterator const a_position(
				io_container.find(i_key));
			if (io_container.end() != a_position)
			{
				a_element.swap(a_position->second);
				io_container.erase(a_position);
			}
		}
		return a_element;
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
	template<
		typename t_hash,
		typename t_real,
		typename t_string,
		typename t_allocator >
	void swap(
		psyq::event_stage< t_hash, t_real, t_string, t_allocator >& io_left,
		psyq::event_stage< t_hash, t_real, t_string, t_allocator >& io_right)
	{
		io_left.swap(io_right);
	}
};

#endif // !PSYQ_SCENE_EVENT_STAGE_HPP_
