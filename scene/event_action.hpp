#ifndef PSYQ_SCENE_EVENT_ACTION_HPP_
#define PSYQ_SCENE_EVENT_ACTION_HPP_

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-package��p�ӂ���event�B
class event_RESERVE_PACKAGE:
	public psyq::scene_world::event::action
{
	typedef event_RESERVE_PACKAGE this_type;
	typedef psyq::scene_world::event::action super_type;

	//-------------------------------------------------------------------------
	public: static psyq::scene_world::event::hash::value get_hash()
	{
		return psyq::scene_world::event::hash::generate("RESERVE_PACKAGE");
	}

	//-------------------------------------------------------------------------
	public: virtual void apply(
		psyq::scene_world&                     io_world,
		psyq::scene_world::event::point const& i_point,
		psyq::scene_world::event::line::scale::value const)
	{
		// world��package��p�ӁB
		io_world.get_package(io_world.event_.replace_hash(i_point.integer));
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-token��animation��ݒ肷��event�B
class event_SET_TOKEN_ANIMATION:
	public psyq::scene_world::event::action
{
	typedef event_SET_TOKEN_ANIMATION this_type;
	typedef psyq::scene_world::event::action super_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		psyq::scene_world::event::item::offset token;
		psyq::scene_world::event::item::offset package;
		psyq::scene_world::event::item::offset flags;
		psyq::scene_world::event::real         start;
	};

	//-------------------------------------------------------------------------
	public: static psyq::scene_world::event::hash::value get_hash()
	{
		return psyq::scene_world::event::hash::generate("SET_TOKEN_ANIMATION");
	}

	//-------------------------------------------------------------------------
	public: virtual void apply(
		psyq::scene_world&                     io_world,
		psyq::scene_world::event::point const& i_point,
		psyq::scene_world::event::line::scale::value const)
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
					//psyq_extern::set_animation(a_token->scene_, *a_package);
				}
			}
		}
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-token��model��ݒ肷��event�B
class event_SET_TOKEN_MODEL:
	public psyq::scene_world::event::action
{
	typedef event_SET_TOKEN_MODEL this_type;
	typedef psyq::scene_world::event::action super_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		psyq::scene_world::event::item::offset token;   ///< token���̏���offset�l�B
		psyq::scene_world::event::item::offset package; ///< package���̏���offset�l�B
	};

	//-------------------------------------------------------------------------
	public: static psyq::scene_world::event::hash::value get_hash()
	{
		return psyq::scene_world::event::hash::generate("SET_TOKEN_MODEL");
	}

	//-------------------------------------------------------------------------
	public: virtual void apply(
		psyq::scene_world&                     io_world,
		psyq::scene_world::event::point const& i_point,
		psyq::scene_world::event::line::scale::value const)
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
struct event_SET_SECTION_CAMERA
{
	psyq::scene_world::event::item::offset section;      ///< section���̏���offset�l�B
	psyq::scene_world::event::item::offset camera_token; ///< camera�Ɏg��token���̏���offset�l�B
	psyq::scene_world::event::item::offset camera_node;  ///< camera�Ɏg��node���̏���offset�l�B
	psyq::scene_world::event::item::offset focus_token;  ///< focus�Ɏg��token���̏���offset�l�B
	psyq::scene_world::event::item::offset focus_node;   ///< focus�Ɏg��node���̏���offset�l�B
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-section��light��ݒ肷��event�B
class event_SET_SECTION_LIGHT:
	public psyq::scene_world::event::action
{
	typedef event_SET_SECTION_LIGHT this_type;
	typedef psyq::scene_world::event::action super_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		psyq::scene_world::event::item::offset section; ///< section�̖��O�B
		psyq::scene_world::event::item::offset token;   ///< light�Ƃ��Ďg��token�̖��O�B
	};

	//-------------------------------------------------------------------------
	public: static psyq::scene_world::event::hash::value get_hash()
	{
		return psyq::scene_world::event::hash::generate("SET_SECTION_LIGHT");
	}

	//-------------------------------------------------------------------------
	public: virtual void apply(
		psyq::scene_world&                     io_world,
		psyq::scene_world::event::point const& i_point,
		psyq::scene_world::event::line::scale::value const)
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
class event_RESERVE_TOKEN:
	public psyq::scene_world::event::action
{
	typedef event_RESERVE_TOKEN this_type;
	typedef psyq::scene_world::event::action super_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		psyq::scene_world::event::item::offset section; ///< section���̏���offset�l�B
		psyq::scene_world::event::item::offset token;   ///< �ǉ�����token���̏���offset�l�B
	};

	//-------------------------------------------------------------------------
	public: static psyq::scene_world::event::hash::value get_hash()
	{
		return psyq::scene_world::event::hash::generate("RESERVE_TOKEN");
	}

	//-------------------------------------------------------------------------
	public: virtual void apply(
		psyq::scene_world&                     io_world,
		psyq::scene_world::event::point const& i_point,
		psyq::scene_world::event::line::scale::value const)
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
class event_REMOVE_TOKEN:
	public psyq::scene_world::event::action
{
	typedef event_REMOVE_TOKEN this_type;
	typedef psyq::scene_world::event::action super_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		psyq::scene_world::event::item::offset section; ///< section���̏���offset�l�B
		psyq::scene_world::event::item::offset token;   ///< �폜����token���̏���offset�l�B
	};

	//-------------------------------------------------------------------------
	public: static psyq::scene_world::event::hash::value get_hash()
	{
		return psyq::scene_world::event::hash::generate("REMOVE_TOKEN");
	}

	//-------------------------------------------------------------------------
	public: virtual void apply(
		psyq::scene_world&                     io_world,
		psyq::scene_world::event::point const& i_point,
		psyq::scene_world::event::line::scale::value const)
	{
		// ���ɂ���������擾�B
		this_type::parameters const* const a_parameters(
			io_world.event_.get_address< this_type::parameters >(
				i_point.integer));
		if (NULL != a_parameters)
		{
			psyq::scene_world::event::hash::value const a_token(
				io_world.event_.replace_hash(a_parameters->token));
			psyq::scene_world::event::hash::value const a_section(
				io_world.event_.replace_hash(a_parameters->section));
			if (psyq::scene_world::event::hash::EMPTY != a_section)
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
