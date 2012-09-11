#ifndef PSYQ_SCENE_EVENT_ACTION_HPP_
#define PSYQ_SCENE_EVENT_ACTION_HPP_

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-package��p�ӂ���event�B
template< typename t_hash, typename t_real, typename t_string >
class psyq::event_action< t_hash, t_real, t_string >::reserve_package:
	public psyq::event_action< t_hash, t_real, t_string >
{
	typedef typename psyq::event_action< t_hash, t_real, t_string >::reserve_package this_type;
	typedef typename psyq::event_action< t_hash, t_real, t_string > super_type;

	//-------------------------------------------------------------------------
	public: static typename t_hash::value get_hash()
	{
		return t_hash::generate("reserve_package");
	}

	//-------------------------------------------------------------------------
	public: virtual void apply(
		psyq::scene_world&                         io_world,
		psyq::event_point< t_hash, t_real > const& i_point,
		t_real const)
	{
		// world��package��p�ӁB
		io_world.get_package(io_world.event_.replace_hash(i_point.integer));
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-token��animation��ݒ肷��event�B
template< typename t_hash, typename t_real, typename t_string >
class psyq::event_action< t_hash, t_real, t_string >::set_token_animation:
	public psyq::event_action< t_hash, t_real, t_string >
{
	typedef typename psyq::event_action< t_hash, t_real, t_string >::set_token_animation this_type;
	typedef typename psyq::event_action< t_hash, t_real, t_string > super_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		typename psyq::event_item< t_hash >::offset token;
		typename psyq::event_item< t_hash >::offset package;
		typename psyq::event_item< t_hash >::offset flags;
		t_real                                      start;
	};

	//-------------------------------------------------------------------------
	public: static typename t_hash::value get_hash()
	{
		return t_hash::generate("set_token_animation");
	}

	//-------------------------------------------------------------------------
	public: virtual void apply(
		psyq::scene_world&                         io_world,
		psyq::event_point< t_hash, t_real > const& i_point,
		t_real const i_time)
	{
		// ���ɂ���������擾�B
		this_type::parameters const* const a_parameters(
			io_world.event_.get_address< this_type::parameters >(
				i_point.integer));
		if (NULL != a_parameters)
		{
			// world����animation-package���擾�B
			psyq::scene_package* const a_package(
				io_world.get_package(
					io_world.event_.replace_hash(
						a_parameters->package)).get());
			if (NULL != a_package)
			{
				// world����token���擾���Aanimation��ݒ�B
				psyq::scene_world::token* const a_token(
					io_world.get_token(
						io_world.event_.replace_hash(
							a_parameters->token)).get());
				if (NULL != a_token)
				{
					//psyq_extern::set_animation(
					//	a_token->scene_, *a_package, i_time);
				}
			}
		}
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-token��model��ݒ肷��event�B
template< typename t_hash, typename t_real, typename t_string >
class psyq::event_action< t_hash, t_real, t_string >::set_token_model:
	public psyq::event_action< t_hash, t_real, t_string >
{
	typedef typename psyq::event_action< t_hash, t_real, t_string >::set_token_model this_type;
	typedef typename psyq::event_action< t_hash, t_real, t_string > super_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		typename psyq::event_item< t_hash >::offset token;   ///< token���̏���offset�l�B
		typename psyq::event_item< t_hash >::offset package; ///< package���̏���offset�l�B
	};

	//-------------------------------------------------------------------------
	public: static typename t_hash::value get_hash()
	{
		return t_hash::generate("set_token_model");
	}

	//-------------------------------------------------------------------------
	public: virtual void apply(
		psyq::scene_world&                         io_world,
		psyq::event_point< t_hash, t_real > const& i_point,
		t_real const)
	{
		// ���ɂ���������擾�B
		this_type::parameters const* const a_parameters(
			io_world.event_.get_address< this_type::parameters >(
				i_point.integer));
		if (NULL != a_parameters)
		{
			// world����model-package���擾�B
			psyq::scene_package* const a_package(
				io_world.get_package(
					io_world.event_.replace_hash(
						a_parameters->package)).get());
			if (NULL != a_package)
			{
				// world����token���擾���Amodel��ݒ�B
				psyq::scene_world::token* const a_token(
					io_world.get_token(
						io_world.event_.replace_hash(
							a_parameters->token)).get());
				if (NULL != a_token)
				{
					psyq_extern::set_model(a_token->scene_, *a_package);
				}
			}
		}
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-section��camera��ݒ肷��event�B
template< typename t_hash, typename t_real, typename t_string >
class psyq::event_action< t_hash, t_real, t_string >::set_section_camera
{
	typename psyq::event_item< t_hash >::offset section;      ///< section���̏���offset�l�B
	typename psyq::event_item< t_hash >::offset camera_token; ///< camera�Ɏg��token���̏���offset�l�B
	typename psyq::event_item< t_hash >::offset camera_node;  ///< camera�Ɏg��node���̏���offset�l�B
	typename psyq::event_item< t_hash >::offset focus_token;  ///< focus�Ɏg��token���̏���offset�l�B
	typename psyq::event_item< t_hash >::offset focus_node;   ///< focus�Ɏg��node���̏���offset�l�B
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-section��light��ݒ肷��event�B
template< typename t_hash, typename t_real, typename t_string >
class psyq::event_action< t_hash, t_real, t_string >::set_section_light:
	public psyq::event_action< t_hash, t_real, t_string >
{
	typedef typename psyq::event_action< t_hash, t_real, t_string >::set_section_light this_type;
	typedef typename psyq::event_action< t_hash, t_real, t_string > super_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		typename psyq::event_item< t_hash >::offset section; ///< section�̖��O�B
		typename psyq::event_item< t_hash >::offset token;   ///< light�Ƃ��Ďg��token�̖��O�B
	};

	//-------------------------------------------------------------------------
	public: static typename t_hash::value get_hash()
	{
		return t_hash::generate("set_section_light");
	}

	//-------------------------------------------------------------------------
	public: virtual void apply(
		psyq::scene_world&                         io_world,
		psyq::event_point< t_hash, t_real > const& i_point,
		t_real const)
	{
		// ���ɂ���������擾�B
		this_type::parameters const* const a_parameters(
			io_world.event_.get_address< this_type::parameters >(
				i_point.integer));
		if (NULL != a_parameters)
		{
			// world����section���擾�B
			psyq::scene_world::section* const a_section(
				io_world.get_section(
					io_world.event_.replace_hash(a_parameters->section)).get());
			if (NULL != a_section)
			{
				// world����light-token���������Asection�ɐݒ�B
				psyq::scene_world::token::shared_ptr const& a_token(
					io_world.get_token(
						io_world.event_.replace_hash(a_parameters->token)));
				if (NULL != a_token.get())
				{
					a_section->light_ = a_token;
				}
			}
		}
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-token��p�ӂ���event�B
template< typename t_hash, typename t_real, typename t_string >
class psyq::event_action< t_hash, t_real, t_string >::reserve_token:
	public psyq::event_action< t_hash, t_real, t_string >
{
	typedef typename psyq::event_action< t_hash, t_real, t_string >::reserve_token this_type;
	typedef typename psyq::event_action< t_hash, t_real, t_string > super_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		typename psyq::event_item< t_hash >::offset section; ///< section���̏���offset�l�B
		typename psyq::event_item< t_hash >::offset token;   ///< �ǉ�����token���̏���offset�l�B
	};

	//-------------------------------------------------------------------------
	public: static typename t_hash::value get_hash()
	{
		return t_hash::generate("reserve_token");
	}

	//-------------------------------------------------------------------------
	public: virtual void apply(
		psyq::scene_world&                         io_world,
		psyq::event_point< t_hash, t_real > const& i_point,
		t_real const)
	{
		// ���ɂ���������擾�B
		this_type::parameters const* const a_parameters(
			io_world.event_.get_address< this_type::parameters >(
				i_point.integer));
		if (NULL != a_parameters)
		{
			// section��token��world�ɗp�ӁB
			io_world.get_token(
				io_world.event_.replace_hash(a_parameters->token),
				io_world.event_.replace_hash(a_parameters->section));
		}
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-token���폜����event�B
template< typename t_hash, typename t_real, typename t_string >
class psyq::event_action< t_hash, t_real, t_string >::remove_token:
	public psyq::event_action< t_hash, t_real, t_string >
{
	typedef typename psyq::event_action< t_hash, t_real, t_string >::remove_token this_type;
	typedef typename psyq::event_action< t_hash, t_real, t_string > super_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		typename psyq::event_item< t_hash >::offset section; ///< section���̏���offset�l�B
		typename psyq::event_item< t_hash >::offset token;   ///< �폜����token���̏���offset�l�B
	};

	//-------------------------------------------------------------------------
	public: static typename t_hash::value get_hash()
	{
		return t_hash::generate("remove_token");
	}

	//-------------------------------------------------------------------------
	public: virtual void apply(
		psyq::scene_world&              io_world,
		psyq::event_point< t_hash, t_real > const& i_point,
		t_real const)
	{
		// ���ɂ���������擾�B
		this_type::parameters const* const a_parameters(
			io_world.event_.get_address< this_type::parameters >(
				i_point.integer));
		if (NULL != a_parameters)
		{
			typename t_hash::value const a_token(
				io_world.event_.replace_hash(a_parameters->token));
			typename t_hash::value const a_section(
				io_world.event_.replace_hash(a_parameters->section));
			if (t_hash::EMPTY != a_section)
			{
				// section����token���폜�B
				io_world.remove_token(a_token, a_section);
			}
			else
			{
				// ���ׂĂ�section����token���폜�B
				io_world.remove_token(a_token);
			}
		}
	}
};

#endif // !PSYQ_SCENE_EVENT_ACTION_HPP_
