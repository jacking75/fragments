#ifndef PSYQ_SCENE_EVENT_STAGE_HPP_
#define PSYQ_SCENE_EVENT_STAGE_HPP_

//#include <psyq/const_string.hpp>
//#include <psyq/scene/event_line.hpp>

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
	public: typedef psyq::event_package< t_hash > package;
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
	/** @param[in] i_package   event-stage���g��event-package�B
	    @param[in] i_allocator �������Ɏg��memory�����q�B
	 */
	public: event_stage(
		typename this_type::package::const_shared_ptr const& i_package,
		t_allocator const&                                   i_allocator):
	package_(i_package),
	actions_(typename this_type::action_map::key_compare(), i_allocator),
	words_(typename this_type::word_map::key_compare(), i_allocator),
	lines_(typename this_type::line_map::key_compare(), i_allocator),
	scales_(typename this_type::scale_map::key_compare(), i_allocator)
	{
		// pass
	}

	//-------------------------------------------------------------------------
	/** @brief �l�������B
	    @param[in,out] ��������ΏہB
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
	/** @brief event-action��}���B
	    @param[in] i_name   �}������event-action�̖��Ohash�l�B
		@param[in] i_action �}������event-action�B
	    @return �}������time-scale�B�}���Ɏ��s�����ꍇ�͋�B
	 */
	public: typename this_type::action::shared_ptr const& insert_action(
		typename t_hash::value const                  i_name,
		typename this_type::action::shared_ptr const& i_action)
	{
		return this_type::package::_insert_shared_ptr(
			this->actions_, i_name, i_action);
	}

	/** @brief event-action���������B
	    @param t_action ����������event-action�̌^�B
	    @return �ǉ�����event-action�ւ̋��Lpointer�B
	 */
	public: template< typename t_action >
	typename this_type::action::shared_ptr const& make_action()
	{
		return this->insert_action(
			t_action::get_hash(),
			PSYQ_ALLOCATE_SHARED< t_action >(this->actions_.get_allocator()));
	}

	public: template< typename t_action, typename t_param0 >
	typename this_type::action::shared_ptr const& make_action(
		t_param0 const& i_param0)
	{
		return this->insert_action(
			t_action::get_hash(),
			PSYQ_ALLOCATE_SHARED< t_action >(
				this->actions_.get_allocator(), i_param0));
	}

	public: template< typename t_action, typename t_param0, typename t_param1 >
	typename this_type::action::shared_ptr const& make_action(
		t_param0 const& i_param0,
		t_param1 const& i_param1)
	{
		return this->insert_action(
			t_action::get_hash(),
			PSYQ_ALLOCATE_SHARED< t_action >(
				this->actions_.get_allocator(), i_param0, i_param1));
	}

	/** @brief event-action�������B
	    @tparam t_action ��������event-action�̌^�B
	    @return ������event-action�B������Ȃ������ꍇ�͋�B
	 */
	public: template< typename t_action >
	typename this_type::action::shared_ptr const& find_action() const
	{
		return this_type::package::_find_shared_ptr(
			this->actions_, t_action::get_hash());
	}

	/** @brief event-action����菜���B
	    @tparam t_action ��������event-action�̌^�B
	    @return ��菜����event-action�B��菜���Ȃ������ꍇ�͋�B
	 */
	public: template< typename t_action >
	typename this_type::action::shared_ptr const remove_action() const
	{
		return this_type::package::_remove_shared_ptr(
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

	/** @brief event-line����菜���B
	    @param[in] i_line ��菜��event-line�̖��Ohash�l�B
	 */
	public: void remove_line(typename t_hash::value const i_line)
	{
		this->lines_.erase(i_line);
	}

	//-------------------------------------------------------------------------
	/** @brief time-scale��}���B
	    @param[in] i_name   �}������time-scale�̖��Ohash�l�B
		@param[in] i_screen �}������time-scale�B
	    @return �}������time-scale�B�}���Ɏ��s�����ꍇ�͋�B
	 */
	public: typename this_type::line::scale::shared_ptr const& insert_scale(
		typename t_hash::value const                       i_name,
		typename this_type::line::scale::shared_ptr const& i_scale)
	{
		return this_type::event::package::_insert_shared_ptr(
			this->scales_, i_name, i_scale);
	}

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
		return this_type::package::_find_shared_ptr(this->scales_, i_scale);
	}

	/** @brief time-scale����菜���B
	    @param[in] i_scale ��菜��time-scale�̖��Ohash�l�B
	    @return ��菜����time-scale�B��菜���Ȃ������ꍇ�͋�B
	 */
	public: typename this_type::line::scale::shared_ptr const remove_scale(
		typename t_hash::value const i_scale)
	{
		typename this_type::line::scale::shared_ptr const a_scale(
			this_type::package::_remove_shared_ptr(this->scales_, i_scale));
		if (NULL != a_scale.get())
		{
			// event-line�W������time-scale����菜���B
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
	    @param[in] i_key  �u�������P��B
	    @param[in] i_word �u��������̒P��B
	    @return �u�������P���hash�l�B
	 */
	public: typename t_hash::value make_word(
		typename this_type::const_string const& i_key,
		typename this_type::const_string const& i_word)
	{
		typename t_hash::value const a_key(t_hash::generate(i_key));
		if (t_hash::EMPTY != a_key)
		{
			this->words_[a_key].assign(i_word.data(), i_word.length());
		}
		return a_key;
	}

	/** @brief event�u�����o�^�B
	    @param[in] i_key  �u�������P��B
	    @param[in] i_word �u��������̒P��B
	    @return �u�������P���hash�l�B
	 */
	public: template< typename t_other_string >
	typename t_hash::value make_word(
		typename t_other_string const&    i_key,
		typename this_type::string const& i_word)
	{
		typename t_hash::value const a_key(t_hash::generate(i_key));
		if (t_hash::EMPTY != a_key)
		{
			this->words_[a_key] = i_word;
		}
		return a_key;
	}

	//-------------------------------------------------------------------------
	/** @brief event-package���̕�������Aevent�u���ꎫ���Œu�������������hash�l���擾�B
	    @param[in] i_offset �u�����ƂȂ镶�����event-package��offset�l�B
	    @return �u����̕������hash�l�B
	 */
	public: typename t_hash::value make_hash(
		typename this_type::package::offset const i_offset)
	const
	{
		return t_hash::generate(this->make_string(i_offset));
	}

	/** @brief �C�ӂ̕�������Aevent�u���ꎫ���Œu�������������hash�l���擾�B
	    @param[in] i_source �u�����ƂȂ镶����B
	    @return �u����̕������hash�l�B
	 */
	public: typename t_hash::value make_hash(
		typename this_type::const_string const& i_source)
	const
	{
		return t_hash::generate(this->make_string(i_source));
	}

	//-------------------------------------------------------------------------
	/** @brief event-package���̕�������Aevent�u���ꎫ���Œu��������������擾�B
	    @param[in] i_offset �u�����ƂȂ镶�����event-package��offset�l�B
	    @return �u����̕�����B
	 */
	public: t_string make_string(
		typename this_type::package::offset const i_offset)
	const
	{
		return this->make_string(this->get_string(i_offset));
	}

	/** @brief �C�ӂ̕�������Aevent�u���ꎫ���Œu��������������擾�B
	    @param[in] i_string �u�����ƂȂ镶����B
	    @return �u����̕�����B
	 */
	public: t_string make_string(
		typename this_type::const_string const& i_string)
	const
	{
		return this_type::replace_string_word(i_string, this->words_);
	}

	//-------------------------------------------------------------------------
	/** @brief event-package���擾�B
	 */
	public:
	typename this_type::package::const_shared_ptr const& get_package() const
	{
		return this->package_;
	}

	/** @brief event-package���̕�������擾�B
	    @param[in] i_offset �������event-package��offset�l�B
	    @return event-package���̕�����B
	 */
	public: typename this_type::const_string get_string(
		typename this_type::package::offset const i_offset)
	const
	{
		// ���������擾���A�������Ԃ��B
		typedef typename PSYQ_MAKE_UNSIGNED< typename t_hash::value >::type
			length_type;
		length_type const* const a_length(
			this->get_value< length_type >(i_offset));
		return NULL != a_length?
			typename this_type::const_string(
				reinterpret_cast< typename t_string::const_pointer >(
					a_length + 1),
				*a_length):
			typename this_type::const_string();
	}

	/** @brief event-package���̒l���擾�B
	    @tparam    t_value  �l�̌^�B
	    @param[in] i_offset �l��event-package��offset�l�B
	    @retval !=NULL event-package���̒l�ւ�pointer�B
	    @retval ==NULL ���s�B
	 */
	public: template< typename t_value >
	t_value const* get_value(
		typename this_type::package::offset const i_offset)
	const
	{
		typename this_type::package const* const a_package(
			this->package_.get());
		return NULL != a_package?
			a_package->template get_value< t_value >(i_offset): NULL;
	}

	/** @brief event-package���̒l���擾�B
	    @tparam    t_value �l�̌^�B
	    @param[in] i_name  �l�̖��Ohash�l�B
	    @retval !=NULL event-package���̒l�ւ�pointer�B
	    @retval ==NULL ���s�B
	 */
	public: template< typename t_value >
	t_value const* find_value(
		typename t_hash::value const i_name)
	const
	{
		typename this_type::package const* const a_package(
			this->package_.get());
		return NULL != a_package?
			a_package->template find_value< t_value >(i_name): NULL;
	}

	//-------------------------------------------------------------------------
	/** @brief event�u���ꎫ������āA�������u���B
	    @param[in] i_string �u�����ƂȂ镶����B
	    @param[in] i_words  �u���ꎫ���B
	    @return �u����̕�����B
	 */
	private: static t_string replace_string_word(
		typename this_type::const_string const& i_string,
		typename this_type::word_map const&     i_words)
	{
		t_string a_string(i_words.get_allocator());
		typename this_type::const_string::const_iterator a_last_end(
			i_string.begin());
		for (;;)
		{
			// �u�����ƂȂ镶���񂩂�A'('��')'�ň͂܂ꂽ�͈͂������B
			typename this_type::const_string const a_word(
				this_type::find_string_word(a_last_end, i_string.end()));
			if (a_word.empty())
			{
				// ���ׂĂ̒P���u�������B
				a_string.append(a_last_end, i_string.end());
				return a_string;
			}

			// ��������u����������B
			typename this_type::word_map::const_iterator const a_position(
				i_words.find(
					t_hash::generate(a_word.begin() + 1, a_word.end() - 1)));
			if (i_words.end() != a_position)
			{
				// �����ɂ���P��Œu������B
				a_string.append(a_last_end, a_word.begin());
				a_string.append(
					a_position->second.begin(),
					a_position->second.end());
			}
			else
			{
				// �u����ł͂Ȃ������̂ŁA���̂܂܂ɂ��Ă����B
				a_string.append(a_last_end, a_word.end());
			}
			a_last_end = a_word.end();
		}
	}

	/** @biref �����񂩂�'('��')'�ň͂܂ꂽ�P��������B
	    @param[in] i_begin �����͈͂̐擪�ʒu�B
	    @param[in] i_end   �����͈̖͂����ʒu�B
	    @return '('��')'�ň͂܂ꂽ�P��B
	 */
	private: static typename this_type::const_string find_string_word(
		typename this_type::const_string::const_iterator const i_begin,
		typename this_type::const_string::const_iterator const i_end)
	{
		typename this_type::const_string::const_iterator a_word_begin(i_end);
		for (
			typename this_type::const_string::const_iterator i = i_begin;
			i_end != i;
			++i)
		{
			switch (*i)
			{
				case '(':
				a_word_begin = i;
				break;

				case ')':
				// �Ή�����'('������΁A�P��͈̔͂�Ԃ��B
				if (i_end != a_word_begin)
				{
					return typename this_type::const_string(
						a_word_begin, i + 1);
				}
				break;
			}
		}
		return typename this_type::const_string(i_end, i_end);
	}

	//-------------------------------------------------------------------------
	private: typename this_type::package::const_shared_ptr package_;

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
