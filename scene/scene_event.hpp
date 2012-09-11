#ifndef PSYQ_SCENE_EVENT_HPP_
#define PSYQ_SCENE_EVENT_HPP_

namespace psyq
{
	class scene_world;
	template< typename, typename, typename > class event_action;
	template< typename, typename, typename > class scene_event;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// event-action�̊��class�B
template< typename t_hash, typename t_real, typename t_string >
class psyq::event_action
{
	typedef psyq::event_action< t_hash, t_real, t_string > this_type;

	public: typedef t_hash hash; ///< event���ɂŎg���Ă���hash�֐��B
	public: typedef t_real real; ///< event���ɂŎg���Ă�������̌^�B
	public: typedef t_string string; ///< ������̌^�B
	public: typedef PSYQ_SHARED_PTR< this_type > shared_ptr;
	public: typedef PSYQ_WEAK_PTR< this_type > weak_ptr;
	public: typedef PSYQ_SHARED_PTR< this_type const > const_shared_ptr;
	public: typedef PSYQ_WEAK_PTR< this_type const > const_weak_ptr;

	// �p�ӂ���Ă���event-action�B
	public: class reserve_package;
	public: class set_token_animation;
	public: class set_token_model;
	public: class set_section_camera;
	public: class set_section_light;
	public: class reserve_token;
	public: class remove_token;

	protected: event_action() {}
	public: virtual ~event_action() {}

	/** event-action��K�p�B
	    @param[in,out] io_world �K�p�Ώۂ�scene-world�B
	    @param[in]     i_point  event�𔭐�������point�B
	    @param[in]     i_time   event��K�p�������ƂɌo�߂��鎞�ԁB
	 */
	public: virtual void apply(
		psyq::scene_world&                         io_world,
		psyq::event_point< t_hash, t_real > const& i_point,
		t_real const                               i_time) = 0;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief scene��event�S�̂��Ǘ�����B
 */
template< typename t_hash, typename t_real, typename t_string >
class psyq::scene_event
{
	typedef psyq::scene_event< t_hash, t_real, t_string > this_type;

	//-------------------------------------------------------------------------
	public: typedef t_hash hash; ///< event���ɂŎg���Ă���hash�֐��B
	public: typedef t_real real; ///< event���ɂŎg���Ă�������̌^�B
	public: typedef t_string string; ///< ������̌^�B
	public: typedef psyq::basic_const_string<
		typename t_string::value_type, typename t_string::traits_type >
			const_string; ///< ������萔�̌^�B
	public: typedef typename t_string::allocator_type allocator;
	public: typedef psyq::file_buffer archive; ///< event���ɁB
	public: typedef psyq::event_item< t_hash > item;
	public: typedef psyq::event_point< t_hash, t_real > point;
	public: typedef psyq::event_line< t_hash, t_real > line;
	public: typedef psyq::event_action< t_hash, t_real, t_string > action;

	//-------------------------------------------------------------------------
	// event�u����̎����B
	public: typedef std::map<
		typename t_hash::value,
		t_string,
		std::less< typename t_hash::value >,
		typename this_type::allocator::rebind<
			std::pair< typename t_hash::value const, t_string > >::other >
				word_map;

	// event-line�̎����B
	public: typedef std::map<
		typename t_hash::value,
		typename this_type::line,
		std::less< typename t_hash::value >,
		typename this_type::allocator::rebind<
			std::pair<
				typename t_hash::value const,
				typename this_type::line > >::other >
					line_map;

	/// event-action�̎����B
	public: typedef std::map<
		typename t_hash::value,
		typename this_type::action::shared_ptr,
		std::less< typename t_hash::value >,
		typename this_type::allocator::rebind<
			std::pair<
				typename t_hash::value const,
				typename this_type::action::shared_ptr > >::other >
					action_map;

	//-------------------------------------------------------------------------
	/** @brief scene-event���\�z�B
	    @param[in] i_archive   �g�p����event���ɁB
	    @param[in] i_allocator �������Ɏg��memory�����q�B
	 */
	public: template< typename t_other_allocator >
	scene_event(
		PSYQ_SHARED_PTR< typename this_type::archive const > const& i_archive,
		t_other_allocator const&                                    i_allocator):
	archive_(i_archive),
	words_(typename this_type::word_map::key_compare(), i_allocator),
	lines_(typename this_type::line_map::key_compare(), i_allocator),
	actions_(typename this_type::action_map::key_compare(), i_allocator)
	{
		// pass
	}

	//-------------------------------------------------------------------------
	/** @brief event�S�̂������B
	    @param[in,out] io_target ��������event�S�́B
	 */
	public: void swap(this_type& io_target)
	{
		this->words_.swap(io_target.words_);
		this->lines_.swap(io_target.lines_);
		this->actions_.swap(io_target.actions_);
		this->archive_.swap(io_target.archive_);
	}

	//-------------------------------------------------------------------------
	/** @brief event�u�����ǉ��B
	    @param[in] i_key  �u�������P��B
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
	    @param[in] i_points ���ɂɂ���event-point�z��̖��Ohash�l�B
	    @param[in] i_key    event-line�����ɓo�^����ۂ�key�B
	    @return �ǉ�����event-line�ւ�pointer�B
	 */
	public: typename this_type::line* add_line(
		typename t_hash::value const                 i_points,
		typename this_type::line_map::key_type const i_key)
	{
		// ������event-line���������猟���B
		typename this_type::line_map::iterator a_position(
			this->lines_.lower_bound(i_key));
		if (this->lines_.end() == a_position || a_position->first != i_key)
		{
			// �V����event-line�������ɒǉ��B
			typename this_type::line a_line(this->archive_, i_points);
			if (a_line.is_stop())
			{
				return NULL;
			}
			a_position = this->lines_.insert(
				a_position,
				typename this_type::line_map::value_type(i_key, a_line));
		}
		else if (!a_position->second.reset(this->archive_, i_points))
		{
			// ������event-line�̏������Ɏ��s�B
			return NULL;
		}
		return &a_position->second;
	}

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
	    @param[in] i_offset �ϊ����镶�����event���ɓ�offset�l�B
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
		return typename this_type::item::replace_word< t_string >(
			this->words_, i_source.begin(), i_source.end());
	}

	//-------------------------------------------------------------------------
	/** @brief ���ɂɑ��݂��镶������擾�B
	    @param[in] i_offset �������event���ɓ�offset�l�B
	 */
	public: typename this_type::const_string get_string(
		typename this_type::item::offset const i_offset)
	const
	{
		// ������̐擪�ʒu���擾�B
		t_string::const_pointer const a_begin(
			this->get_address< t_string::value_type >(i_offset));
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
	/** @brief event���ɂɑ��݂���l�ւ�pointer���擾�B
	    @tparam    t_value  �l�̌^�B
	    @param[in] i_offset �l��event����offset�l�B
	 */
	public: template< typename t_value >
	t_value const* get_address(
		typename this_type::item::offset const i_offset) const
	{
		typename this_type::item::archive const* const a_archive(
			this->archive_.get());
		return NULL != a_archive?
			typename this_type::item::get_address< t_value >(
				*a_archive, i_offset):
			NULL;
	}

	//-------------------------------------------------------------------------
	/** @brief event���ɂ��擾�B
	 */
	public: PSYQ_SHARED_PTR< typename this_type::archive const > const&
	get_archive() const
	{
		return this->archive_;
	}

	//-------------------------------------------------------------------------
	/// event���ɁB
	private: PSYQ_SHARED_PTR< typename this_type::archive const > archive_;

	public: typename this_type::word_map   words_;   ///< event�u����̎����B
	public: typename this_type::line_map   lines_;   ///< event-line�̎����B
	public: typename this_type::action_map actions_; ///< event-action�̎����B
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
