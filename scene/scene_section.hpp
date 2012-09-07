#ifndef PSYQ_SCENE_SECTION_HPP_
#define PSYQ_SCENE_SECTION_HPP_

namespace psyq
{
	template< typename, typename > class scene_section;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
template< typename t_hash, typename t_real >
class psyq::scene_section:
	private boost::noncopyable
{
	typedef psyq::scene_section< t_hash, t_real > this_type;

	//-------------------------------------------------------------------------
	public: typedef PSYQ_SHARED_PTR< this_type > shared_ptr;
	public: typedef PSYQ_WEAK_PTR< this_type > weak_ptr;

	//-------------------------------------------------------------------------
	public: typedef t_hash hash;
	public: typedef t_real real;
	public: typedef psyq::scene_token< t_hash, t_real > token;
	public: typedef psyq::scene_camera< t_hash, t_real > camera;

	//-------------------------------------------------------------------------
	private: typedef std::vector<
		typename this_type::token::shared_ptr,
		typename psyq_extern::allocator::template rebind<
			typename this_type::token::shared_ptr >::other >
				token_container;

	//-------------------------------------------------------------------------
	public: template< typename t_allocator >
	explicit scene_section(t_allocator const& i_allocator):
	tokens_(i_allocator)
	{
		// pass
	}

	//-------------------------------------------------------------------------
	/** @brief scene-token��ǉ��B
	    @param[in] i_token �ǉ�����scene-token�B
	 */
	public: bool add_token(
		typename this_type::token::shared_ptr const& i_token)
	{
		if (NULL == i_token.get())
		{
			return false;
		}
		if (!this->find_token(i_token))
		{
			this->tokens_.push_back(i_token);
		}
		return true;
	}

	/** @brief scene-tokens�������B
	    @param[in] i_token ��������scene-token�B
	    @return true�Ȃ猩�������Bfalse�Ȃ猩����Ȃ������B
	 */
	public: bool find_token(
		typename this_type::token::shared_ptr const& i_token)
	const
	{
		return this->find_token_index(i_token.get()) < this->tokens_.size();
	}

	/** @brief scene-token���폜�B
	    @param[in] i_token �폜����scene-token�B
	 */
	public: bool remove_token(
		typename this_type::token::shared_ptr const& i_token)
	{
		std::size_t const a_index(this->find_token_index(i_token.get()));
		if (a_index < this->tokens_.size())
		{
			this->tokens_.at(a_index).swap(this->tokens_.back());
			this->tokens_.pop_back();
			return true;
		}
		return false;
	}

	//-------------------------------------------------------------------------
	/** @brief render-target�ɕ`�悷��B
	    @param[in] i_target �`���render-target�B
	    @param[in] i_camera
	        �`��Ɏg��camera�BNULL�̏ꍇ��set_camera()�Ŏw�肳�ꂽcamera���g���B
	    @param[in] i_light
	        �`��Ɏg��light�BNULL�̏ꍇ��set_light()�Ŏw�肳�ꂽlight���g���B
	 */
	public: void draw(
		psyq::render_target const&           i_target,
		psyq_extern::scene_node const* const i_camera = NULL,
		psyq_extern::scene_unit const* const i_light = NULL)
	{
		PSYQ_ASSERT(i_target.is_drawing());

		// camera��ݒ�B
		psyq_extern::scene_node const* const a_camera(
			psyq_extern::set_camera(i_camera, this->camera_.get()));

		// light��ݒ�B
		psyq_extern::set_light(i_light, this->light_.get());

		// �`��B
		if (NULL != a_camera)
		{
			psyq_extern::draw_tokens(this->tokens_, i_target);
		}
	}

	//-------------------------------------------------------------------------
	private: std::size_t find_token_index(
		typename this_type::token const* const i_token)
	const
	{
		if (NULL != i_token)
		{
			for (std::size_t i = 0; i < this->tokens_.size(); ++i)
			{
				if (this->tokens_.at(i).get() == i_token)
				{
					return i;
				}
			}
		}
		return (std::numeric_limits< std::size_t >::max)();
	}

	//-------------------------------------------------------------------------
	public:  typename this_type::camera::shared_ptr camera_;
	public:  typename this_type::token::shared_ptr  light_;
	private: typename this_type::token_container    tokens_;
};

#endif // !PSYQ_SCENE_SECTION_HPP_
