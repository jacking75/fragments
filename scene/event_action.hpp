#ifndef DSSG_SCENE_EVENT_ACTION_HPP_
#define DSSG_SCENE_EVENT_ACTION_HPP_

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-package��file����ǂݍ���event�B
class event_LOAD_PACKAGE:
	public psyq::scene_event::action
{
	typedef event_LOAD_PACKAGE this_type;
	typedef psyq::scene_event::action super_type;

//.............................................................................
public:
	//-------------------------------------------------------------------------
	struct parameters
	{
		psyq::scene_event::item::offset package_name; ///< package���̏���offset�l�B
		psyq::scene_event::item::offset scene_path;   ///< scene��path���̏���offset�l�B
		psyq::scene_event::item::offset shader_path;  ///< shader��path���̏���offset�l�B
		psyq::scene_event::item::offset texture_path; ///< texture��path���̏���offset�l�B
	};

	//-------------------------------------------------------------------------
	static psyq::scene_event::hash::value get_hash()
	{
		return psyq::scene_event::hash::generate("LOAD_PACKAGE");
	}

	//-------------------------------------------------------------------------
	virtual void apply(
		psyq::scene_world&              io_world,
		psyq::scene_event::point const& i_point,
		psyq::scene_event::line::time_scale::value const)
	{
		this_type::parameters const* const a_parameters(
			io_world.event_.get_address< this_type::parameters >(
				i_point.integer));
		if (NULL != a_parameters)
		{
			io_world.load_package(
				io_world.event_.replace_hash(a_parameters->package_name),
				io_world.event_.get_string(a_parameters->scene_path),
				io_world.event_.get_string(a_parameters->shader_path),
				io_world.event_.get_string(a_parameters->texture_path));
		}
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-token��animation��ݒ肷��event�B
struct event_SET_TOKEN_ANIMATION
{
	psyq::scene_event::item::offset token;
	psyq::scene_event::item::offset package;
	psyq::scene_event::item::offset flags;
	psyq::scene_event::real         start;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-token��model��ݒ肷��event�B
class event_SET_TOKEN_MODEL:
	public psyq::scene_event::action
{
	typedef event_SET_TOKEN_MODEL this_type;
	typedef psyq::scene_event::action super_type;

//.............................................................................
public:
	//-------------------------------------------------------------------------
	struct parameters
	{
		psyq::scene_event::item::offset token;   ///< token���̏���offset�l�B
		psyq::scene_event::item::offset package; ///< package���̏���offset�l�B
	};

	//-------------------------------------------------------------------------
	static psyq::scene_event::hash::value get_hash()
	{
		return psyq::scene_event::hash::generate("SET_TOKEN_MODEL");
	}

	//-------------------------------------------------------------------------
	virtual void apply(
		psyq::scene_world&              io_world,
		psyq::scene_event::point const& i_point,
		psyq::scene_event::line::time_scale::value const)
	{
		this_type::parameters const* const a_parameters(
			io_world.event_.get_address< this_type::parameters >(
				i_point.integer));
		if (NULL != a_parameters)
		{
			psyq::scene_world::package_map::const_iterator const
				a_position(
					io_world.packages_.find(
						io_world.event_.replace_hash(a_parameters->package)));
			psyq::scene_package const* const a_package(
				io_world.packages_.end() != a_position?
					a_position->second.get(): NULL);
			if (NULL != a_package)
			{
				// scene-token���擾�B
				psyq::scene_token::shared_ptr const a_token(
					io_world.add_token(
						io_world.event_.replace_hash(a_parameters->token)));
				if (NULL != a_token.get())
				{
				}
			}
		}
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-section��camera��ݒ肷��event�B
struct event_SET_SECTION_CAMERA
{
	psyq::scene_event::item::offset section;      ///< section���̏���offset�l�B
	psyq::scene_event::item::offset camera_token; ///< camera�Ɏg��token���̏���offset�l�B
	psyq::scene_event::item::offset camera_node;  ///< camera�Ɏg��node���̏���offset�l�B
	psyq::scene_event::item::offset focus_token;  ///< focus�Ɏg��token���̏���offset�l�B
	psyq::scene_event::item::offset focus_node;   ///< focus�Ɏg��node���̏���offset�l�B
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-section��light��ݒ肷��event�B
class event_SET_SECTION_LIGHT:
	public psyq::scene_event::action
{
	typedef event_SET_SECTION_LIGHT this_type;
	typedef psyq::scene_event::action super_type;

//.............................................................................
public:
	//-------------------------------------------------------------------------
	struct parameters
	{
		psyq::scene_event::item::offset section; ///< section�̖��O�B
		psyq::scene_event::item::offset token;   ///< light�Ƃ��Ďg��token�̖��O�B
	};

	//-------------------------------------------------------------------------
	static psyq::scene_event::hash::value get_hash()
	{
		return psyq::scene_event::hash::generate("SET_SECTION_LIGHT");
	}

	//-------------------------------------------------------------------------
	virtual void apply(
		psyq::scene_world&              io_world,
		psyq::scene_event::point const& i_point,
		psyq::scene_event::line::time_scale::value const)
	{
		this_type::parameters const* const a_parameters(
			io_world.event_.get_address< this_type::parameters >(
				i_point.integer));
		if (NULL != a_parameters)
		{
			psyq::scene_section::shared_ptr const a_section(
				io_world.add_section(
					io_world.event_.replace_hash(a_parameters->section)));
			if (NULL != a_section.get())
			{
				psyq::scene_token::shared_ptr a_token(
					io_world.add_token(
						io_world.event_.replace_hash(a_parameters->token)));
				if (NULL != a_token.get())
				{
					a_section->light_.swap(a_token);
				}
			}
		}
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-token��ǉ�����event�B
class event_ADD_TOKEN:
	public psyq::scene_event::action
{
	typedef event_ADD_TOKEN this_type;
	typedef psyq::scene_event::action super_type;

//.............................................................................
public:
	//-------------------------------------------------------------------------
	struct parameters
	{
		psyq::scene_event::item::offset section; ///< section���̏���offset�l�B
		psyq::scene_event::item::offset token;   ///< �ǉ�����token���̏���offset�l�B
	};

	//-------------------------------------------------------------------------
	static psyq::scene_event::hash::value get_hash()
	{
		return psyq::scene_event::hash::generate("ADD_TOKEN");
	}

	//-------------------------------------------------------------------------
	virtual void apply(
		psyq::scene_world&              io_world,
		psyq::scene_event::point const& i_point,
		psyq::scene_event::line::time_scale::value const)
	{
		this_type::parameters const* const a_parameters(
			io_world.event_.get_address< this_type::parameters >(
				i_point.integer));
		if (NULL != a_parameters)
		{
			psyq::scene_event::hash::value const a_token(
				io_world.event_.replace_hash(a_parameters->token));
			psyq::scene_event::hash::value const a_section(
				io_world.event_.replace_hash(a_parameters->section));
			if (psyq::scene_event::hash::EMPTY != a_section)
			{
				// section��token��ǉ��B
				io_world.add_token(a_token, a_section);
			}
			else
			{
				// world��token��ǉ��B
				io_world.add_token(a_token);
			}
		}
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-token���폜����event�B
class event_REMOVE_TOKEN:
	public psyq::scene_event::action
{
	typedef event_REMOVE_TOKEN this_type;
	typedef psyq::scene_event::action super_type;

//.............................................................................
public:
	//-------------------------------------------------------------------------
	struct parameters
	{
		psyq::scene_event::item::offset section; ///< section���̏���offset�l�B
		psyq::scene_event::item::offset token;   ///< �폜����token���̏���offset�l�B
	};

	//-------------------------------------------------------------------------
	static psyq::scene_event::hash::value get_hash()
	{
		return psyq::scene_event::hash::generate("REMOVE_TOKEN");
	}

	//-------------------------------------------------------------------------
	virtual void apply(
		psyq::scene_world&              io_world,
		psyq::scene_event::point const& i_point,
		psyq::scene_event::line::time_scale::value const)
	{
		this_type::parameters const* const a_parameters(
			io_world.event_.get_address< this_type::parameters >(
				i_point.integer));
		if (NULL != a_parameters)
		{
			psyq::scene_event::hash::value const a_token(
				io_world.event_.replace_hash(a_parameters->token));
			psyq::scene_event::hash::value const a_section(
				io_world.event_.replace_hash(a_parameters->section));
			if (psyq::scene_event::hash::EMPTY != a_section)
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

#endif // !DSSG_SCENE_EVENT_ACTION_HPP_
