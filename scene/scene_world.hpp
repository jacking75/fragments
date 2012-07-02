#ifndef PSYQ_SCENE_WORLD_HPP_
#define PSYQ_SCENE_WORLD_HPP_

namespace psyq
{
	class scene_world;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
class psyq::scene_world
{
	typedef psyq::scene_world this_type;

//.............................................................................
public:
	//-------------------------------------------------------------------------
	// scene-package�̎����B
	typedef std::map<
		psyq::scene_event::hash::value,
		scene_package::shared_ptr,
		std::less< psyq::scene_event::hash::value >,
		psyq::scene_event::allocator::rebind<
			std::pair<
				psyq::scene_event::hash::value const,
				scene_package::shared_ptr > >::other >
					package_map;

	/// scene-token�̎����B
	typedef std::map<
		psyq::scene_event::hash::value,
		scene_token::shared_ptr,
		std::less< psyq::scene_event::hash::value >,
		psyq::scene_event::allocator::rebind<
			std::pair<
				psyq::scene_event::hash::value const,
				scene_token::shared_ptr > >::other >
					token_map;

	/// scene-section�̎����B
	typedef std::map<
		psyq::scene_event::hash::value,
		scene_section::shared_ptr,
		std::less< psyq::scene_event::hash::value >,
		psyq::scene_event::allocator::rebind<
			std::pair<
				psyq::scene_event::hash::value const,
				scene_section::shared_ptr > >::other >
					section_map;

	//-------------------------------------------------------------------------
	template< typename t_allocator >
	explicit scene_world(t_allocator const& i_allocator):
	event_(i_allocator),
	packages_(this_type::package_map::key_compare(), i_allocator),
	sections_(this_type::section_map::key_compare(), i_allocator),
	tokens_(this_type::token_map::key_compare(), i_allocator)
	{
		// pass
	}

	//-------------------------------------------------------------------------
	/** @brief �X�V�B
	    @param[in] i_frame_time  1frame������̎��ԁB
	    @param[in] i_frame_count �i�߂�frame���B
	 */
	void update(
		psyq_extern::scene_time const&             i_frame_time,
		psyq::scene_event::time_scale::value const i_frame_count = 1)
	{
		// dssg-scene�̎��Ԃ��X�V�B
		this_type::forward_scenes(this->tokens_, i_frame_time, i_frame_count);

		// event���X�V�B
		this_type::dispatch_map a_dispatch(
			this_type::dispatch_map::key_compare(),
			this->event_.lines_.get_allocator());
		this_type::forward_events(
			a_dispatch, this->event_.lines_, i_frame_count);
		this_type::apply_events(*this, a_dispatch, this->event_.actions_);

		// dssg-scene���X�V�B
		this_type::update_scenes(this->tokens_);
	}

	//-------------------------------------------------------------------------
	/** @brief package��ǉ��B
	    @param[in] i_name    �ǉ�����package�̖��Ohash�l�B
	    @param[in] i_package �ǉ�����package�B
	    @return �ǉ�����package�B�ǉ��Ɏ��s�����ꍇ�͋�B
	 */
	psyq::scene_package::shared_ptr add_package(
		psyq::scene_event::hash::value const   i_name,
		psyq::scene_package::shared_ptr const& i_package)
	{
		return this_type::add_element(this->packages_, i_name, i_package);
	}

	/** @brief package�������B
	    @param[in] i_name ��������package�̖��Ohash�l�B
		@return ������pacakge�B������Ȃ������ꍇ�͋�B
	 */
	psyq::scene_package::shared_ptr find_package(
		psyq::scene_event::hash::value const i_name)
	const
	{
		return this_type::find_element(this->packages_, i_name);
	}

	/** @brief package���폜�B
	    @param[in] i_name �폜����package�̖��Ohash�l�B
	    @return �폜����package�B�폜���Ȃ������ꍇ�͋�B
	 */
	psyq::scene_package::shared_ptr remove_package(
		psyq::scene_event::hash::value const i_name)
	{
		return this_type::remove_element(this->packages_, i_name);
	}

	//-------------------------------------------------------------------------
	/** @brief section��ǉ��B
	    section���ɑΉ�����section�����݂��Ȃ��ꍇ�́A�V����section�����B
	    section���ɑΉ�����section�����łɑ��݂���ꍇ�́A�ǉ��͍s���Ȃ��B
	    @param[in] i_name �ǉ�����section�̖��Ohash�l�B
	    @return section���ɑΉ�����section�B�ǉ��Ɏ��s�����ꍇ�͋�B
	 */
	psyq::scene_section::shared_ptr add_section(
		psyq::scene_event::hash::value const i_name)
	{
		
		if (psyq::scene_event::hash::EMPTY == i_name)
		{
			// ��hash�l���ƒǉ��Ɏ��s����B
			return scene_section::shared_ptr();
		}
		psyq::scene_section::shared_ptr& a_section(this->sections_[i_name]);
		if (NULL == a_section.get())
		{
			PSYQ_ALLOCATE_SHARED< psyq::scene_section >(
				this->sections_.get_allocator(),
				this->sections_.get_allocator()).swap(a_section);
			PSYQ_ASSERT(NULL != a_section.get());
		}
		return a_section;
	}

	/** @brief section�������B
	    @param[in] i_name ��������section�̖��Ohash�l�B
	    @return ��������section�B������Ȃ������ꍇ�͋�B
	 */
	psyq::scene_section::shared_ptr find_section(
		psyq::scene_event::hash::value const i_name)
	const
	{
		return this_type::find_element(this->sections_, i_name);
	}

	/** @brief section���폜�B
	    @param[in] i_name �폜����section�̖��Ohash�l�B
	    @return �폜����section�B�폜���Ȃ������ꍇ�͋�B
	 */
	psyq::scene_section::shared_ptr remove_section(
		psyq::scene_event::hash::value const i_name)
	{
		return this_type::remove_element(this->sections_, i_name);
	}

	//-------------------------------------------------------------------------
	/** @brief world��token��ǉ��B
	    token���ɑΉ�����token�����݂��Ȃ��ꍇ�́A�V����token�����B
	    token���ɑΉ�����token�����łɑ��݂���ꍇ�́A�ǉ��͍s���Ȃ��B
	    @param[in] i_token �ǉ�����token�̖��Ohash�l�B
		@return token���ɑΉ�����token�B
	 */
	psyq::scene_token::shared_ptr add_token(
		psyq::scene_event::hash::value const i_name)
	{
		if (psyq::scene_event::hash::EMPTY == i_name)
		{
			// ��hash�l���ƒǉ��Ɏ��s����B
			return scene_token::shared_ptr();
		}
		psyq::scene_token::shared_ptr& a_token(this->tokens_[i_name]);
		if (NULL == a_token.get())
		{
			PSYQ_ALLOCATE_SHARED< psyq::scene_token >(
				this->tokens_.get_allocator()).swap(a_token);
			PSYQ_ASSERT(NULL != a_token.get());
		}
		return a_token;
	}


	/** @brief section��token��ǉ��B
	    token���ɑΉ�����token�����݂��Ȃ��ꍇ�́A�V����token�����B
	    token���ɑΉ�����token�����łɑ��݂���ꍇ�́A�ǉ��͍s���Ȃ��B
	    section���ɑΉ�����section�����݂��Ȃ��ꍇ�́A�V����section�����B
	    @param[in] i_name    �ǉ�����token�̖��Ohash�l�B
	    @param[in] i_section �ΏۂƂȂ�section�̖��Ohash�l�B
		@return token���ɑΉ�����token�B�ǉ��Ɏ��s�����ꍇ�͋�B
	 */
	psyq::scene_token::shared_ptr add_token(
		psyq::scene_event::hash::value const i_name,
		psyq::scene_event::hash::value const i_section)
	{
		// �ΏۂƂȂ�section���擾�B
		psyq::scene_section::shared_ptr const a_section(
			this->add_section(i_section));
		if (NULL != a_section.get())
		{
			// world��token��ǉ��B
			psyq::scene_token::shared_ptr const a_token(
				this->add_token(i_name));
			if (NULL != a_token.get())
			{
				// section��token��ǉ��B
				a_section->add_token(a_token);
				return a_token;
			}
		}
		return psyq::scene_token::shared_ptr();
	}

	/** @brief world����token�������B
	    @param[in] i_name ��������token�̖��Ohash�l�B
	    @return ������token�B������Ȃ������ꍇ�͋�B
	 */
	psyq::scene_token::shared_ptr find_token(
		psyq::scene_event::hash::value const i_name)
	const
	{
		return this_type::find_element(this->tokens_, i_name);
	}

	/** @brief world����token���폜�B
	    @param[in] i_name �폜����token�̖��Ohash�l�B
	    @return �폜����token�B�폜���Ȃ������ꍇ�͋�B
	 */
	psyq::scene_token::shared_ptr remove_token(
		psyq::scene_event::hash::value const i_name)
	{
		psyq::scene_token::shared_ptr a_token;

		// token���擾�B
		this_type::token_map::iterator const a_token_pos(
			this->tokens_.find(i_name));
		if (this->tokens_.end() != a_token_pos)
		{
			// ���ׂĂ�section����token���폜�B
			for (
				this_type::section_map::const_iterator i =
					this->sections_.begin();
				this->sections_.end() != i;
				++i)
			{
				psyq::scene_section* const a_section(i->second.get());
				if (NULL != a_section)
				{
					a_section->remove_token(a_token_pos->second);
				}
			}

			// world����token���폜�B
			a_token.swap(a_token_pos->second);
			this->tokens_.erase(a_token_pos);
		}
		return a_token;
	}

	/** @brief section����token���폜�B
	    @param[in] i_name    �폜����token�̖��Ohash�l�B
	    @param[in] i_section �ΏۂƂȂ�section�̖��Ohash�l�B
	 */
	psyq::scene_token::shared_ptr remove_token(
		psyq::scene_event::hash::value const i_name,
		psyq::scene_event::hash::value const i_section)
	{
		// token���擾�B
		this_type::token_map::const_iterator const a_token_pos(
			this->tokens_.find(i_name));
		if (this->tokens_.end() != a_token_pos)
		{
			// section����Atoken���폜�B
			this_type::section_map::const_iterator const a_section_pos(
				this->sections_.find(i_section));
			if (this->sections_.end() != a_section_pos)
			{
				psyq::scene_section* const a_section(
					a_section_pos->second.get());
				if (NULL != a_section)
				{
					a_section->remove_token(a_token_pos->second);
					return a_token_pos->second;
				}
			}
		}
		return scene_token::shared_ptr();
	}

//.............................................................................
private:
	typedef std::multimap<
		psyq::scene_event::time_scale::value,
		psyq::scene_event::point const*,
		std::greater< psyq::scene_event::time_scale::value >,
		psyq::scene_event::allocator::rebind<
			std::pair<
				psyq::scene_event::time_scale::value const,
				psyq::scene_event::point const* > >::other >
					dispatch_map;

	//-------------------------------------------------------------------------
	template< typename t_value, typename t_string >
	static PSYQ_SHARED_PTR< t_value > load_file(t_string const& i_path)
	{
		i_path.length();
		return PSYQ_SHARED_PTR< t_value >(); // �������Ȃ̂ŁB
	}

	//-------------------------------------------------------------------------
	/** @brief container�ɗv�f��ǉ��B
	    @param[in] i_container �ΏۂƂȂ�container�B
	    @param[in] i_name      �ǉ�����v�f�̖��Ohash�l�B
	    @param[in] i_mapped    �ǉ�����v�f�B
	 */
	template< typename t_container >
	static typename t_container::mapped_type add_element(
		t_container&                             io_container,
		typename t_container::key_type const     i_name,
		typename t_container::mapped_type const& i_mapped)
	{
		if (psyq::scene_event::hash::EMPTY == i_name || NULL == i_mapped.get())
		{
			return typename t_container::mapped_type();
		}
		io_container[i_name] = i_mapped;
		return i_mapped;
	}

	/** @brief container����v�f�������B
	    @param[in] i_container �ΏۂƂȂ�container�B
	    @param[in] i_name      �폜����v�f�̖��Ohash�l�B
	 */
	template< typename t_container >
	static typename t_container::mapped_type find_element(
		t_container const&                   i_container,
		psyq::scene_event::hash::value const i_name)
	{
		typename t_container::const_iterator const a_position(
			i_container.find(i_name));
		return i_container.end() != a_position?
			a_position->second: typename t_container::mapped_type();
	}

	/** @brief container����v�f���폜�B
	    @param[in] i_container �ΏۂƂȂ�container�B
	    @param[in] i_name      �폜����v�f�̖��Ohash�l�B
	 */
	template< typename t_container >
	static typename t_container::mapped_type remove_element(
		t_container&                         io_container,
		psyq::scene_event::hash::value const i_name)
	{
		typename t_container::mapped_type a_mapped;
		typename t_container::iterator const a_position(
			io_container.find(i_name));
		if (io_container.end() != a_position)
		{
			a_mapped.swap(a_position->second);
			io_container.erase(a_position);
		}
		return a_mapped;
	}

	//-------------------------------------------------------------------------
	/** @brief dssg-scene�̎��Ԃ��X�V�B
	    @param[in] i_tokens      dssg-scene������token�̎����B
	    @param[in] i_frame_time  1frame������̎��ԁB
	    @param[in] i_frame_count �i�߂�frame���B
	 */
	static void forward_scenes(
		this_type::token_map const&                i_tokens,
		psyq_extern::scene_time const&             i_frame_time,
		psyq::scene_event::time_scale::value const i_frame_count)
	{
		for (
			this_type::token_map::const_iterator i = i_tokens.begin();
			i_tokens.end() != i;
			++i)
		{
			scene_token* const a_token(i->second.get());
			if (NULL != a_token)
			{
				psyq::scene_event::time_scale::value const a_time_scale(
					NULL != a_token->time_scale_.get()?
						i_frame_count * a_token->time_scale_->get_scale():
						i_frame_count);
				psyq_extern::forward_scene_unit(
					a_token->scene_, i_frame_time, a_time_scale);
			}
		}
	}

	/** @brief dssg-scene���X�V�B
	    @param[in] i_tokens dssg-scene������token�̎����B
	 */
	static void update_scenes(this_type::token_map const& i_tokens)
	{
		for (
			this_type::token_map::const_iterator i = i_tokens.begin();
			i_tokens.end() != i;
			++i)
		{
			psyq::scene_token* const a_token(i->second.get());
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
	static void forward_events(
		this_type::dispatch_map&                   io_dispatch,
		psyq::scene_event::line_map const&         i_lines,
		psyq::scene_event::time_scale::value const i_frame_count)
	{
		for (
			psyq::scene_event::line_map::const_iterator i = i_lines.begin();
			i_lines.end() != i;
			++i)
		{
			psyq::scene_event::line_map::mapped_type& a_line(
				const_cast< psyq::scene_event::line_map::mapped_type& >(
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
	static void apply_events(
		this_type&                           io_world,
		this_type::dispatch_map const&       i_dispatch,
		psyq::scene_event::action_map const& i_actions)
	{
		for (
			this_type::dispatch_map::const_iterator i = i_dispatch.begin();
			i_dispatch.end() != i;
			++i)
		{
			// event-point�ɑΉ�����event�֐�object�������B
			PSYQ_ASSERT(NULL != i->second);
			psyq::scene_event::point const& a_point(*i->second);
			psyq::scene_event::action_map::const_iterator const a_position(
				i_actions.find(a_point.type));
			if (i_actions.end() != a_position)
			{
				psyq::scene_event::action* const a_action(
					a_position->second.get());
				if (NULL != a_action)
				{
					// event��K�p�B
					a_action->apply(io_world, a_point, i->first);
				}
			}
		}
	}

//.............................................................................
public:
	psyq::scene_event       event_;
	this_type::package_map  packages_; ///< scene-package�̎����B
	this_type::section_map  sections_; ///< scene-section�̎����B
	this_type::token_map    tokens_;   ///< scene-token�̎����B
};

#endif // !DSSG_SCENE_WORLD_HPP_
