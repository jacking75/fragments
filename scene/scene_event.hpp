#ifndef PSYQ_SCENE_EVENT_BOOK_HPP_
#define PSYQ_SCENE_EVENT_BOOK_HPP_

namespace psyq
{
	class scene_world;
	class scene_event;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief scene��event�S�̂��Ǘ�����B
 */
class psyq::scene_event
{
	typedef psyq::scene_event this_type;

	//-------------------------------------------------------------------------
	public: typedef psyq_extern::allocator allocator;

	//-------------------------------------------------------------------------
	public: typedef psyq::file_buffer archive;      ///< ���ɁB
	public: typedef psyq::fnv1_hash32 hash;         ///< ���ɂŎg���Ă���hash�֐��B
	public: typedef this_type::hash::value integer; ///< ���ɂŎg���Ă��鐮���̌^�B
	public: typedef float real;                     ///< ���ɂŎg���Ă�������̌^�B
	public: typedef char letter;                    ///< ���ɂŎg���Ă��镶���̌^�B

	//-------------------------------------------------------------------------
	public: typedef psyq::event_item< this_type::hash > item;
	public: typedef psyq::event_point< this_type::hash, this_type::real > point;
	public: typedef psyq::event_line< this_type::hash, this_type::real > line;
	public: typedef this_type::line::time_scale time_scale;

	//-------------------------------------------------------------------------
	public: typedef std::basic_string<
		this_type::letter,
		std::char_traits< this_type::letter >,
		this_type::allocator::rebind< this_type::letter >::other >
			string;
	public: typedef psyq::basic_const_string< this_type::letter > const_string;

	//-------------------------------------------------------------------------
	/// event-action�̊��class�B
	public: class action
	{
		typedef action this_type;

		public: typedef PSYQ_SHARED_PTR< this_type > shared_ptr;
		public: typedef PSYQ_WEAK_PTR< this_type > weak_ptr;
		public: typedef PSYQ_SHARED_PTR< this_type const > const_shared_ptr;
		public: typedef PSYQ_WEAK_PTR< this_type const > const_weak_ptr;

		public: virtual ~action()
		{
			// pass
		}

		/** event-action��K�p�B
	    @param[in,out] io_world �K�p�Ώۂ�scene-world�B
	    @param[in]     i_point  �����ƂȂ���event-point�B
	    @param[in]     i_time   event��K�p�������ƂɌo�߂��鎞�ԁB
		 */
		public: virtual void apply(
			psyq::scene_world&                         io_world,
			psyq::scene_event::point const&            i_point,
			psyq::scene_event::time_scale::value const i_time) = 0;

		protected: action()
		{
			// pass
		}
	};

	//-------------------------------------------------------------------------
	// event�u����̎����B
	public: typedef std::map<
		this_type::hash::value,
		this_type::string,
		std::less< this_type::hash::value >,
		this_type::allocator::rebind<
			std::pair<
				this_type::hash::value const,
				this_type::string > >::other >
					word_map;

	// event-line�̎����B
	public: typedef std::map<
		this_type::hash::value,
		this_type::line,
		std::less< this_type::hash::value >,
		this_type::allocator::rebind<
			std::pair<
				this_type::hash::value const,
				this_type::line > >::other >
					line_map;

	/// event-action�̎����B
	public: typedef std::map<
		this_type::hash::value,
		this_type::action::shared_ptr,
		std::less< this_type::hash::value >,
		this_type::allocator::rebind<
			std::pair<
				this_type::hash::value const,
				this_type::action::shared_ptr > >::other >
					action_map;

	//-------------------------------------------------------------------------
	public: template< typename t_allocator >
	explicit scene_event(t_allocator const& i_allocator):
	words_(this_type::word_map::key_compare(), i_allocator),
	lines_(this_type::line_map::key_compare(), i_allocator),
	actions_(this_type::action_map::key_compare(), i_allocator)
	{
		// pass
	}

	//-------------------------------------------------------------------------
	/** @brief event�u�����ǉ��B
	    @param[in] i_key  �u�������P��B
	    @param[in] i_word �u��������̒P��B
	 */
	public: void add_word(
		this_type::const_string const& i_key,
		this_type::const_string const& i_word)
	{
		this->words_[this_type::hash::generate(i_key)].assign(
			i_word.data(), i_word.length());
	}

	/** @brief event-line��ǉ��B
	    @param[in] i_points ���ɂɂ���event-point�z��̖��Ohash�l�B
	    @param[in] i_key    event-line�����ɓo�^����ۂ�key�B
	 */
	public: this_type::line* add_line(
		this_type::hash::value const        i_points,
		this_type::line_map::key_type const i_key)
	{
		this_type::line_map::iterator a_position(
			this->lines_.lower_bound(i_key));
		if (this->lines_.end() == a_position || a_position->first != i_key)
		{
			this_type::line a_line(this->archive_, i_points);
			if (a_line.is_stop())
			{
				return NULL;
			}
			a_position = this->lines_.insert(
				a_position, this_type::line_map::value_type(i_key, a_line));
		}
		else if (!a_position->second.reset(this->archive_, i_points))
		{
			return NULL;
		}
		return &a_position->second;
	}

	public: template< typename t_action >
	void add_action()
	{
		this->actions_[t_action::get_hash()] =
			PSYQ_ALLOCATE_SHARED< t_action >(this->actions_.get_allocator());
	}

	//-------------------------------------------------------------------------
	/** @brief �u���ꎫ������ď��ɂɑ��݂��镶�����u�����Ahash�l���擾�B
	    @param[in] i_offset �ϊ����镶����̏��ɓ�offset�l�B
	    @return �u����̕������hash�l�B
	 */
	public: this_type::hash::value replace_hash(
		this_type::item::offset const i_offset)
	const
	{
		return this_type::hash::generate(this->replace_string(i_offset));
	}

	/** @brief �u���ꎫ������ĕ������u�����Ahash�l���擾�B
	    @param[in] i_source �u������镶����B
	    @return �u����̕������hash�l�B
	 */
	public: this_type::hash::value replace_hash(
		this_type::const_string const& i_source)
	const
	{
		return this_type::hash::generate(this->replace_string(i_source));
	}

	//-------------------------------------------------------------------------
	/** @brief �u���ꎫ������āA���ɂɑ��݂��镶�����u���B
	    @param[in] i_offset �ϊ����镶�����event���ɓ�offset�l�B
	    @return �u����̕�����B
	 */
	public: this_type::string replace_string(
		this_type::item::offset const i_offset)
	const
	{
		return this->replace_string(this->get_string(i_offset));
	}

	/** @brief �u���ꎫ������āA�������u���B
	    @param[in] i_string �u������镶����B
	    @return �u����̕�����B
	 */
	public: this_type::string replace_string(
		this_type::const_string const& i_source)
	const
	{
		return this_type::item::replace_word< this_type::string >(
			this->words_, i_source);
	}

	//-------------------------------------------------------------------------
	/** @brief ���ɂɑ��݂��镶������擾�B
	    @param[in] i_offset �������event���ɓ�offset�l�B
	 */
	public: this_type::const_string get_string(
		this_type::item::offset const i_offset)
	const
	{
		this_type::const_string::const_pointer const a_string(
			this->get_address< this_type::const_string::value_type >(
				i_offset));
		return NULL != a_string?
			this_type::const_string(
				a_string,
				this_type::const_string::traits_type::length(a_string)):
			this_type::const_string();
	}

	//-------------------------------------------------------------------------
	/** @brief event���ɂɑ��݂���l�ւ�pointer���擾�B
	    @tparam    t_value  �l�̌^�B
	    @param[in] i_offset �l��event����offset�l�B
	 */
	public: template< typename t_value >
	t_value const* get_address(this_type::item::offset const i_offset) const
	{
		this_type::item::archive const* const a_archive(
			this->archive_.get());
		return NULL != a_archive?
			this_type::item::get_address< t_value >(*a_archive, i_offset):
			NULL;
	}

	//-------------------------------------------------------------------------
	/** @brief event���ɂ��擾�B
	 */
	public: PSYQ_SHARED_PTR< this_type::archive const > const& get_archive() const
	{
		return this->archive_;
	}

	//-------------------------------------------------------------------------
	public: this_type::word_map   words_;   ///< event�u����̎����B
	public: this_type::line_map   lines_;   ///< event-line�̎����B
	public: this_type::action_map actions_; ///< event-action�̎����B

	/// event���ɁB
	private: PSYQ_SHARED_PTR< this_type::archive const > archive_;
};

#endif // !PSYQ_SCENE_EVENT_BOOK_HPP_
