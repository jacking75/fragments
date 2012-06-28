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
	/** @brief scene-section��ǉ��B���łɂ���ꍇ�́A�ǉ����Ȃ��B
	    @param[in] i_name �ǉ�����scene-section�̖��Ohash�l�B
	    @return ���Ohash�l�ɑΉ�����scene-section�B
	 */
	scene_section::shared_ptr add_section(
		psyq::scene_event::hash::value const i_name)
	{
		if (psyq::scene_event::hash::EMPTY == i_name)
		{
			return scene_section::shared_ptr();
		}
		this_type::section_map::iterator a_position(
			this->sections_.lower_bound(i_name));
		if (this->sections_.end() == a_position ||
			a_position->first != i_name)
		{
			// �ΏۂƂȂ�scene-section���Ȃ��̂ŁA�V���ɍ��B
			a_position = this->sections_.insert(
				a_position,
				this_type::section_map::value_type(
					i_name,
					PSYQ_ALLOCATE_SHARED< scene_section >(
						this->sections_.get_allocator(),
						this->sections_.get_allocator())));
			PSYQ_ASSERT(this->sections_.end() != a_position);
		}
		PSYQ_ASSERT(NULL != a_position->second.get());
		return a_position->second;
	}

	/** @brief scene-section�������B
	    @param[in] i_name ��������scene-section�̖��Ohash�l�B
	    @return ��������scene-section�B
	 */
	scene_section::shared_ptr find_section(
		psyq::scene_event::hash::value const i_name)
	const
	{
		return this_type::find_element(this->sections_, i_name);
	}

	/** @brief scene-section���폜�B
	    @param[in] i_name �폜����scene-section�̖��Ohash�l�B
	    @return �폜����scene-section�B
	 */
	scene_section::shared_ptr remove_section(
		psyq::scene_event::hash::value const i_name)
	{
		scene_section::shared_ptr a_section;
		this_type::section_map::iterator const a_position(
			this->sections_.find(i_name));
		if (this->sections_.end() != a_position)
		{
			a_section.swap(a_position->second);
			this->sections_.erase(a_position);
		}
		return a_section;
	}

	//-------------------------------------------------------------------------
	/** @brief world��scene-token��ǉ��B
	    @param[in] i_token �ǉ�����scene-token�̖��Ohash�l�B
	 */
	scene_token::shared_ptr add_token(
		psyq::scene_event::hash::value const i_name)
	{
		if (psyq::scene_event::hash::EMPTY == i_name)
		{
			return scene_token::shared_ptr();
		}
		this_type::token_map::iterator a_position(
			this->tokens_.lower_bound(i_name));
		if (this->tokens_.end() == a_position || a_position->first != i_name)
		{
			a_position = this->tokens_.insert(
				a_position,
				this_type::token_map::value_type(
					i_name,
					PSYQ_ALLOCATE_SHARED< scene_token >(
						this->tokens_.get_allocator())));
			PSYQ_ASSERT(this->tokens_.end() != a_position);
		}
		PSYQ_ASSERT(NULL != a_position->second.get());
		return a_position->second;
	}

	/** @brief scene-section��scene-token��ǉ��B
	    @param[in] i_name    �ǉ�����scene-token�̖��Ohash�l�B
	    @param[in] i_section �ΏۂƂȂ�scene-section�̖��Ohash�l�B
	 */
	scene_token::shared_ptr add_token(
		psyq::scene_event::hash::value const i_name,
		psyq::scene_event::hash::value const i_section)
	{
		scene_section::shared_ptr const a_section(this->add_section(i_section));
		if (NULL != a_section.get())
		{
			scene_token::shared_ptr const a_token(this->add_token(i_name));
			if (NULL != a_token.get())
			{
				a_section->add_token(a_token);
				return a_token;
			}
		}
		return scene_token::shared_ptr();
	}

	/** @brief scene-world����scene-token�������B
	    @param[in] i_name ��������scene-token�̖��Ohash�l�B
	 */
	scene_token::shared_ptr find_token(
		psyq::scene_event::hash::value const i_name)
	const
	{
		return this_type::find_element(this->tokens_, i_name);
	}

	/** @brief scene-world����scene-token���폜�B
	    @param[in] i_name �폜����scene-token�̖��Ohash�l�B
	 */
	scene_token::shared_ptr remove_token(
		psyq::scene_event::hash::value const i_name)
	{
		scene_token::shared_ptr a_token;

		// scene-token���擾�B
		this_type::token_map::iterator const a_token_pos(
			this->tokens_.find(i_name));
		if (this->tokens_.end() != a_token_pos)
		{
			// ���ׂĂ�scene-section����Ascene-token���폜�B
			for (
				this_type::section_map::const_iterator i =
					this->sections_.begin();
				this->sections_.end() != i;
				++i)
			{
				scene_section* const a_section(i->second.get());
				if (NULL != a_section)
				{
					a_section->remove_token(a_token_pos->second);
				}
			}

			// scene-world����Ascene-token���폜�B
			a_token.swap(a_token_pos->second);
			this->tokens_.erase(a_token_pos);
		}
		return a_token;
	}

	/** @brief scene-section����scene-token���폜�B
	    @param[in] i_name    �폜����scene-token�̖��Ohash�l�B
	    @param[in] i_section �ΏۂƂȂ�scene-section�̖��Ohash�l�B
	 */
	scene_token::shared_ptr remove_token(
		psyq::scene_event::hash::value const i_name,
		psyq::scene_event::hash::value const i_section)
	{
		// scene-token���擾�B
		this_type::token_map::const_iterator const a_token_pos(
			this->tokens_.find(i_name));
		if (this->tokens_.end() != a_token_pos)
		{
			// scene-section����Ascene-token���폜�B
			this_type::section_map::const_iterator const a_section_pos(
				this->sections_.find(i_section));
			if (this->sections_.end() != a_section_pos)
			{
				scene_section* const a_section(a_section_pos->second.get());
				if (NULL != a_section)
				{
					a_section->remove_token(a_token_pos->second);
					return a_token_pos->second;
				}
			}
		}
		return scene_token::shared_ptr();
	}

	//-------------------------------------------------------------------------
	/** @brief file����package��ǂݍ��ށB
	    @param[in] i_name         �ǂݍ���package�ɂ��閼�Ohash�l�B
	    @param[in] i_scene_path   scene-package��path���B
	    @param[in] i_shader_path  scene-package��path���B
	    @param[in] i_texture_path scene-package��path���B
	 */
	scene_package::shared_ptr load_package(
		psyq::scene_event::hash::value const   i_name,
		psyq::scene_event::const_string const& i_scene_path,
		psyq::scene_event::const_string const& i_shader_path,
		psyq::scene_event::const_string const& i_texture_path)
	{
		psyq::scene_event::string const a_scene_path(
			this->event_.replace_string(i_scene_path));
		if (psyq::scene_event::hash::EMPTY != i_name && !a_scene_path.empty())
		{
			// texture��file����ǂݍ��ށB
			psyq::scene_event::string const a_texture_path(
				this->event_.replace_string(i_texture_path));
			texture_package::shared_ptr a_texture;
			if (!a_texture_path.empty())
			{
				this_type::load_file< texture_package >(
					a_texture_path).swap(a_texture);
				if (NULL == a_texture.get())
				{
					// texture���ǂݍ��߂Ȃ������B
					PSYQ_ASSERT(false);
					return  scene_package::shared_ptr();
				}
			}

			// shader��file����ǂݍ��ށB
			psyq::scene_event::string const a_shader_path(
				this->event_.replace_string(i_shader_path));
			shader_package::shared_ptr a_shader;
			if (!a_shader_path.empty())
			{
				this_type::load_file< shader_package >(
					a_shader_path).swap(a_shader);
				if (NULL == a_shader.get())
				{
					// shader���ǂݍ��߂Ȃ������B
					PSYQ_ASSERT(false);
					return scene_package::shared_ptr();
				}
			}

			// scene��file����ǂݍ��ށB
			scene_package::shared_ptr const a_scene(
				this_type::load_file< scene_package >(a_scene_path));
			if (NULL != a_scene.get())
			{
				// scene-package�����ɓo�^�B
				this->packages_[i_name] = a_scene;
				return a_scene;
			}
			else
			{
				PSYQ_ASSERT(false);
			}
		}
		return scene_package::shared_ptr();
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
	/** @brief container����v�f�������B
	    @param[in] i_container ��������container�B
	    @param[in] i_name      �������̖��Ohash�l�B
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
			scene_token* const a_token(i->second.get());
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
