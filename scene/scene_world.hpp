#ifndef PSYQ_SCENE_WORLD_HPP_
#define PSYQ_SCENE_WORLD_HPP_

namespace psyq
{
	class scene_world;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief scene�S�̂��Ǘ�����B
 */
class psyq::scene_world
{
	typedef psyq::scene_world this_type;

	//-------------------------------------------------------------------------
	private: typedef psyq::fnv1_hash32 t_hash;
	private: typedef float t_real;
	private: typedef std::basic_string<
		char,
		std::char_traits< char >,
		psyq_extern::allocator::rebind< char >::other >
			t_string;

	//-------------------------------------------------------------------------
	public: typedef psyq::scene_event< t_hash, t_real, t_string > event;
	public: typedef psyq::scene_token<
		this_type::event::hash, this_type::event::real >
			token;
	public: typedef psyq::scene_section<
		this_type::event::hash, this_type::event::real >
			section;

	//-------------------------------------------------------------------------
	/// scene-package�̎����B
	public: typedef std::map<
		this_type::event::hash::value,
		psyq::scene_package::shared_ptr,
		std::less< this_type::event::hash::value >,
		this_type::event::allocator::rebind<
			std::pair<
				this_type::event::hash::value const,
				psyq::scene_package::shared_ptr > >::other >
					package_map;

	/// scene-token�̎����B
	public: typedef std::map<
		this_type::event::hash::value,
		this_type::token::shared_ptr,
		std::less< this_type::event::hash::value >,
		this_type::event::allocator::rebind<
			std::pair<
				this_type::event::hash::value const,
				this_type::token::shared_ptr > >::other >
					token_map;

	/// scene-section�̎����B
	public: typedef std::map<
		this_type::event::hash::value,
		this_type::section::shared_ptr,
		std::less< this_type::event::hash::value >,
		this_type::event::allocator::rebind<
			std::pair<
				this_type::event::hash::value const,
				this_type::section::shared_ptr > >::other >
					section_map;

	//-------------------------------------------------------------------------
	private: struct package_path
	{
		this_type::event::item::offset scene;   ///< scene��path���̏���offset�l�B
		this_type::event::item::offset shader;  ///< shader��path���̏���offset�l�B
		this_type::event::item::offset texture; ///< texture��path���̏���offset�l�B
	};

	private: typedef std::multimap<
		this_type::event::line::scale::value,
		this_type::event::point const*,
		std::greater< this_type::event::line::scale::value >,
		this_type::event::allocator::rebind<
			std::pair<
				this_type::event::line::scale::value const,
				this_type::event::point const* > >::other >
					dispatch_map;

	//-------------------------------------------------------------------------
	public: template< typename t_allocator >
	scene_world(
		PSYQ_SHARED_PTR< this_type::event::archive const > const& i_archive,
		t_allocator const&                                        i_allocator):
	event_(i_archive, i_allocator),
	packages_(this_type::package_map::key_compare(), i_allocator),
	sections_(this_type::section_map::key_compare(), i_allocator),
	tokens_(this_type::token_map::key_compare(), i_allocator)
	{
		// pass
	}

	//-------------------------------------------------------------------------
	/** @brief world�S�̂������B
	    @param[in,out] io_target ��������world�S�́B
	 */
	public: void swap(this_type& io_target)
	{
		this->event_.swap(io_target.event_);
		this->packages_.swap(io_target.packages_);
		this->sections_.swap(io_target.sections_);
		this->tokens_.swap(io_target.tokens_);
	}

	//-------------------------------------------------------------------------
	/** @brief �X�V�B
	    @param[in] i_frame_time  1frame������̎��ԁB
	    @param[in] i_frame_count �i�߂�frame���B
	 */
	public: void update(
		psyq_extern::scene_time const&             i_frame_time,
		this_type::event::line::scale::value const i_frame_count = 1)
	{
		// scene�̎��Ԃ��X�V�B
		this_type::forward_scenes(this->tokens_, i_frame_time, i_frame_count);

		// event���X�V�B
		this_type::dispatch_map a_dispatch(
			this_type::dispatch_map::key_compare(),
			this->event_.lines_.get_allocator());
		this_type::forward_events(
			a_dispatch, this->event_.lines_, i_frame_count);
		this_type::apply_events(*this, a_dispatch, this->event_.actions_);

		// scene���X�V�B
		this_type::update_scenes(this->tokens_);
	}

	//-------------------------------------------------------------------------
	/** @brief package���擾�B
	    package���ɑΉ�����package�����݂��Ȃ��ꍇ�́A
	    file����ǂݍ���Œǉ�����B
	    @param[in] i_name �擾����package�̖��Ohash�l�B
	    @return package���ɑΉ�����package�B�擾�Ɏ��s�����ꍇ�͋�B
	 */
	public: psyq::scene_package::shared_ptr const& get_package(
		this_type::event::hash::value const i_name)
	{
		if (this_type::event::hash::EMPTY != i_name)
		{
			// ������packae�������B
			psyq::scene_package::shared_ptr& a_package(
				this->packages_[i_name]);
			if (NULL != a_package.get())
			{
				// package�̎擾�ɐ����B
				return a_package;
			}

			// file����package��ǂݍ��ށB
			this->load_package(i_name).swap(a_package);
			if (NULL != a_package.get())
			{
				// package�̎擾�ɐ����B
				return a_package;
			}
			PSYQ_ASSERT(false);
			this->packages_.erase(i_name);
		}

		// package�̎擾�Ɏ��s�B
		return this_type::get_null_ptr< psyq::scene_package >();
	}

	/** @brief package�������B
	    @param[in] i_name ��������package�̖��Ohash�l�B
	    @return ������pacakge�B������Ȃ������ꍇ�͋�B
	 */
	public: psyq::scene_package::shared_ptr const& find_package(
		this_type::event::hash::value const i_name)
	const
	{
		return this_type::find_element(this->packages_, i_name);
	}

	/** @brief package���폜�B
	    @param[in] i_name �폜����package�̖��Ohash�l�B
	    @return �폜����package�B�폜���Ȃ������ꍇ�͋�B
	 */
	public: psyq::scene_package::shared_ptr remove_package(
		this_type::event::hash::value const i_name)
	{
		return this_type::remove_element(this->packages_, i_name);
	}

	//-------------------------------------------------------------------------
	/** @brief section���擾�B
	    section���ɑΉ�����section�����݂��Ȃ��ꍇ�́A�V����section�����B
	    @param[in] i_name �擾����section�̖��Ohash�l�B
	    @return section���ɑΉ�����section�B�擾�Ɏ��s�����ꍇ�͋�B
	 */
	public: this_type::section::shared_ptr const& get_section(
		this_type::event::hash::value const i_name)
	{
		if (this_type::event::hash::EMPTY != i_name)
		{
			// ������section���猟���B
			this_type::section::shared_ptr& a_section(
				this->sections_[i_name]);
			if (NULL != a_section.get())
			{
				// section�̎擾�ɐ����B
				return a_section;
			}

			// �V����section�����B
			PSYQ_ALLOCATE_SHARED< this_type::section >(
				this->sections_.get_allocator(),
				this->sections_.get_allocator()).swap(a_section);
			if (NULL != a_section.get())
			{
				// section�̎擾�ɐ����B
				return a_section;
			}
			PSYQ_ASSERT(false);
			this->sections_.erase(i_name);
		}

		// section�̎擾�Ɏ��s�B
		return this_type::get_null_ptr< this_type::section >();
	}

	/** @brief section�������B
	    @param[in] i_name ��������section�̖��Ohash�l�B
	    @return ��������section�B������Ȃ������ꍇ�͋�B
	 */
	public: this_type::section::shared_ptr const& find_section(
		this_type::event::hash::value const i_name)
	const
	{
		return this_type::find_element(this->sections_, i_name);
	}

	/** @brief section���폜�B
	    @param[in] i_name �폜����section�̖��Ohash�l�B
	    @return �폜����section�B�폜���Ȃ������ꍇ�͋�B
	 */
	public: this_type::section::shared_ptr remove_section(
		this_type::event::hash::value const i_name)
	{
		return this_type::remove_element(this->sections_, i_name);
	}

	//-------------------------------------------------------------------------
	/** @brief world����token���擾�B
	    token���ɑΉ�����token�����݂��Ȃ��ꍇ�́A�V����token�����B
	    @param[in] i_token �擾����token�̖��Ohash�l�B
	    @return token���ɑΉ�����token�B�擾�Ɏ��s�����ꍇ�͋�B
	 */
	public: this_type::token::shared_ptr const& get_token(
		this_type::event::hash::value const i_name)
	{
		if (this_type::event::hash::EMPTY != i_name)
		{
			// ������token���猟���B
			this_type::token::shared_ptr& a_token(this->tokens_[i_name]);
			if (NULL != a_token.get())
			{
				return a_token;
			}

			// �V����token�����B
			PSYQ_ALLOCATE_SHARED< this_type::token >(
				this->tokens_.get_allocator()).swap(a_token);
			if (NULL != a_token.get())
			{
				return a_token;
			}
			PSYQ_ASSERT(false);
			this->tokens_.erase(i_name);
		}
		return this_type::get_null_ptr< this_type::token >();
	}

	/** @brief section����token���擾�B
	    token���ɑΉ�����token�����݂��Ȃ��ꍇ�́A�V����token�����B
	    section���ɑΉ�����section�����݂��Ȃ��ꍇ�́A�V����section�����B
	    section��token���Ȃ��ꍇ�́Asection��token��ǉ�����B
	    @param[in] i_token   �擾����token�̖��Ohash�l�B
	    @param[in] i_section �ΏۂƂȂ�section�̖��Ohash�l�B
	    @return token���ɑΉ�����token�B�擾�Ɏ��s�����ꍇ�͋�B
	 */
	public: this_type::token::shared_ptr const& get_token(
		this_type::event::hash::value const i_token,
		this_type::event::hash::value const i_section)
	{
		// token��section���擾�B
		this_type::token::shared_ptr const& a_token(this->get_token(i_token));
		this_type::section* const a_section(
			this->get_section(i_section).get());

		// section��token��ǉ��B
		return NULL != a_section && a_section->add_token(a_token)?
			a_token: this_type::get_null_ptr< this_type::token >();
	}

	/** @brief world����token�������B
	    @param[in] i_name ��������token�̖��Ohash�l�B
	    @return ������token�B������Ȃ������ꍇ�͋�B
	 */
	public: this_type::token::shared_ptr const& find_token(
		this_type::event::hash::value const i_name)
	const
	{
		return this_type::find_element(this->tokens_, i_name);
	}

	/** @brief world��section����token���폜�B
	    @param[in] i_name �폜����token�̖��Ohash�l�B
	    @return �폜����token�B�폜���Ȃ������ꍇ�͋�B
	 */
	public: this_type::token::shared_ptr remove_token(
		this_type::event::hash::value const i_name)
	{
		this_type::token::shared_ptr a_token;

		// token���擾�B
		this_type::token_map::iterator const a_token_pos(
			this->tokens_.find(i_name));
		if (this->tokens_.end() != a_token_pos)
		{
			// world����token���폜�B
			a_token.swap(a_token_pos->second);
			this->tokens_.erase(a_token_pos);

			// ���ׂĂ�section����token���폜�B
			for (
				this_type::section_map::const_iterator i =
					this->sections_.begin();
				this->sections_.end() != i;
				++i)
			{
				this_type::section* const a_section(i->second.get());
				if (NULL != a_section)
				{
					a_section->remove_token(a_token);
				}
			}
		}
		return a_token;
	}

	/** @brief section����token���폜�B
	    @param[in] i_token   �폜����token�̖��Ohash�l�B
	    @param[in] i_section �ΏۂƂȂ�section�̖��Ohash�l�B
	    @return �폜����token�B�폜���Ȃ������ꍇ�͋�B
	 */
	public: this_type::token::shared_ptr const& remove_token(
		this_type::event::hash::value const i_token,
		this_type::event::hash::value const i_section)
	{
		// section�������B
		this_type::section_map::const_iterator const a_section_pos(
			this->sections_.find(i_section));
		if (this->sections_.end() != a_section_pos)
		{
			// token�������B
			this_type::token_map::const_iterator const a_token_pos(
				this->tokens_.find(i_token));
			if (this->tokens_.end() != a_token_pos)
			{
				// section����Atoken���폜�B
				this_type::section* const a_section(
					a_section_pos->second.get());
				if (NULL != a_section &&
					a_section->remove_token(a_token_pos->second))
				{
					return a_token_pos->second;
				}
			}
		}
		return this_type::get_null_ptr< this_type::token >();
	}

	//-------------------------------------------------------------------------
	/** @brief container����v�f�������B
	    @param[in] i_container �ΏۂƂȂ�container�B
	    @param[in] i_name      �폜����v�f�̖��Ohash�l�B
	 */
	private: template< typename t_container >
	static typename t_container::mapped_type const& find_element(
		t_container const&                  i_container,
		this_type::event::hash::value const i_name)
	{
		typename t_container::const_iterator const a_position(
			i_container.find(i_name));
		return i_container.end() != a_position?
			a_position->second:
			this_type::get_null_ptr<
				typename t_container::mapped_type::element_type >();
	}

	/** @brief container����v�f���폜�B
	    @param[in] i_container �ΏۂƂȂ�container�B
	    @param[in] i_name      �폜����v�f�̖��Ohash�l�B
	 */
	private: template< typename t_container >
	static typename t_container::mapped_type remove_element(
		t_container&                        io_container,
		this_type::event::hash::value const i_name)
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
	/** @brief file����pacakge��ǂݍ��ށB
	    @param[in] i_name package�̖��Ohash�l�B
	 */
	private: psyq::scene_package::shared_ptr load_package(
		this_type::event::hash::value const i_name)
	const
	{
		// ���ɂ���package-path�������B
		this_type::event::item const* const a_item(
			this_type::event::item::find(
				*this->event_.get_archive(), i_name));
		if (NULL != a_item)
		{
			this_type::package_path const* const a_path(
				this->event_.get_address< this_type::package_path >(
					a_item->begin));
			if (NULL != a_path)
			{
				// file����package��ǂݍ��ށB
				psyq::scene_package::shared_ptr const a_package(
					psyq::scene_package::load(
						this->packages_.get_allocator(),
						this->event_.replace_string(a_path->scene),
						this->event_.replace_string(a_path->shader),
						this->event_.replace_string(a_path->texture)));
				if (NULL != a_package.get())
				{
					return a_package;
				}
			}
		}
		return this_type::get_null_ptr< psyq::scene_package >();
	}

	//-------------------------------------------------------------------------
	/** @brief scene�̎��Ԃ��X�V�B
	    @param[in] i_tokens      scene������token�̎����B
	    @param[in] i_frame_time  1frame������̎��ԁB
	    @param[in] i_frame_count �i�߂�frame���B
	 */
	private: static void forward_scenes(
		this_type::token_map const&                i_tokens,
		psyq_extern::scene_time const&             i_frame_time,
		this_type::event::line::scale::value const i_frame_count)
	{
		for (
			this_type::token_map::const_iterator i = i_tokens.begin();
			i_tokens.end() != i;
			++i)
		{
			this_type::token* const a_token(i->second.get());
			if (NULL != a_token)
			{
				this_type::event::line::scale::value const a_time_scale(
					NULL != a_token->time_scale_.get()?
						i_frame_count * a_token->time_scale_->get_scale():
						i_frame_count);
				psyq_extern::forward_scene_unit(
					a_token->scene_, i_frame_time, a_time_scale);
			}
		}
	}

	/** @brief scene���X�V�B
	    @param[in] i_tokens scene������token�̎����B
	 */
	private: static void update_scenes(
		this_type::token_map const& i_tokens)
	{
		for (
			this_type::token_map::const_iterator i = i_tokens.begin();
			i_tokens.end() != i;
			++i)
		{
			this_type::token* const a_token(i->second.get());
			if (NULL != a_token)
			{
				psyq_extern::update_scene_unit(a_token->scene_);
			}
		}
	}

	//-------------------------------------------------------------------------
	/** @brief event-line�̎��Ԃ��X�V���A��������event��container�ɓo�^�B
	    @param[in,out] io_dispatch   ��������event��o�^����container�B
	    @param[in]     i_lines       �X�V����event-line�̎����B
	    @param[in]     i_frame_count �i�߂�frame���B
	 */
	private: static void forward_events(
		this_type::dispatch_map&                   io_dispatch,
		this_type::event::line_map const&          i_lines,
		this_type::event::line::scale::value const i_frame_count)
	{
		for (
			this_type::event::line_map::const_iterator i = i_lines.begin();
			i_lines.end() != i;
			++i)
		{
			this_type::event::line_map::mapped_type& a_line(
				const_cast< this_type::event::line_map::mapped_type& >(
					i->second));
			a_line.seek(i_frame_count, SEEK_CUR);
			a_line.dispatch(io_dispatch);
		}
	}

	/** @brief container�ɓo�^����Ă���event�ɑΉ�����֐����Ăяo���B
	    @param[in,out] io_world   event�K�p�Ώۂ�world�B
	    @param[in]     i_dispatch ��������event���o�^����Ă���container�B
	    @param[in]     i_actions  event-action�̎����B
	 */
	private: static void apply_events(
		this_type&                          io_world,
		this_type::dispatch_map const&      i_dispatch,
		this_type::event::action_map const& i_actions)
	{
		for (
			this_type::dispatch_map::const_iterator i = i_dispatch.begin();
			i_dispatch.end() != i;
			++i)
		{
			// event-point�ɑΉ�����event�֐�object�������B
			PSYQ_ASSERT(NULL != i->second);
			this_type::event::point const& a_point(*i->second);
			this_type::event::action_map::const_iterator const a_position(
				i_actions.find(a_point.type));
			if (i_actions.end() != a_position)
			{
				this_type::event::action* const a_action(
					a_position->second.get());
				if (NULL != a_action)
				{
					// event��K�p�B
					a_action->apply(io_world, a_point, i->first);
				}
			}
		}
	}

	//-------------------------------------------------------------------------
	private: template< typename t_value >
	static PSYQ_SHARED_PTR< t_value > const& get_null_ptr()
	{
		static PSYQ_SHARED_PTR< t_value > const s_null_ptr;
		return s_null_ptr;
	}

	//-------------------------------------------------------------------------
	public: this_type::event       event_;    ///< scene-event�̊Ǘ��B
	public: this_type::package_map packages_; ///< scene-package�̎����B
	public: this_type::section_map sections_; ///< scene-section�̎����B
	public: this_type::token_map   tokens_;   ///< scene-token�̎����B
};

//-----------------------------------------------------------------------------
namespace std
{
	void swap(psyq::scene_world& io_left, psyq::scene_world& io_right)
	{
		io_left.swap(io_right);
	}
};

#endif // !PSYQ_SCENE_WORLD_HPP_
