/** @file
    @brief morton������p������ԕ����؂ɂ��Փ˔���B

    �g�����́A psyq::mosp_handle �� psyq::mosp_tree
    �̏ڍא������Q�Ƃ��邱�ƁB

    �ȉ���web-page���Q�l�ɂ��Ď��������B
    http://marupeke296.com/COL_2D_No8_QuadTree.html
 */
#ifndef PSYQ_MOSP_COLLISION_HPP_
#define PSYQ_MOSP_COLLISION_HPP_
#include <glm/glm.hpp> // OpenGL Mathematics
//#include "psyq/bit_algorithm.hpp"

namespace psyq
{
    /// @cond
    template<typename> class mosp_handle;
    template<typename> class mosp_node;
    template<typename> class mosp_coordinates_xyz;
    template<typename> class mosp_coordinates_xz;
    template<typename> class mosp_space;
    template<typename> class mosp_space_2d;
    template<typename> class mosp_space_3d;
    template<
        typename, typename = mosp_space_2d<mosp_coordinates_xz<glm::vec3>>>
            class mosp_tree;
    /// @endcond
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief morton�����ɂ���ԕ����؂�p�����Փ˔����handle�B

    �g�����̊T�v�B
    -# �Փ˂��镨�̂̎��ʒl���A mosp_handle::object_ �ɑ�����Ă����B
    -# mosp_handle::Attach() ���Ăяo���A
       �Փ˔���handle����ԕ����؂Ɏ�����B
    -# mosp_tree::detect_collision() ���Ăяo���A�Փ˂𔻒肷��B

    @tparam template_collision_object @copydoc mosp_handle::collision_object
 */
template<typename template_collision_object>
class psyq::mosp_handle
{
    /// *this�̌^�B
    private: typedef mosp_handle<template_collision_object> self;

    /// �Փ˂��镨�̂̎��ʒl�̌^�B
    public: typedef template_collision_object collision_object;

    //-------------------------------------------------------------------------
    /** @brief �Փ˔���handle���\�z����B
        @param[in] in_object this�ɑΉ�����A�Փ˕��̂̎��ʒl�̏����l�B
     */
    public: explicit mosp_handle(
        template_collision_object in_object)
    :
        object_(std::move(in_object)),
        node_(nullptr)
    {}

    /// copy-constructor�͎g�p�֎~�B
    private: mosp_handle(self const&);

    /** @brief move-constructor�B
        @param[in,out] io_source �ړ����ƂȂ�instance�B
     */
    public: mosp_handle(
        self&& io_source)
    :
        object_(std::move(io_source.object_)),
        node_(io_source.node_)
    {
        if (io_source.node_ != nullptr)
        {
            io_source.node_ = nullptr;
            this->node_->handle_ = this;
        }
    }

    /// @brief ��ԕ����؂���this�����O���B
    public: ~mosp_handle()
    {
        this->detach_tree();
    }

    /// copy������Z�q�͎g�p�֎~�B
    private: self& operator=(const self&);

    /** @brief move������Z�q�B
        @param[in,out] io_source �ړ����ƂȂ�instance�B
     */
    public: self& operator=(self&& io_source)
    {
        if (this != &io_source)
        {
            this->~self();
            new(this) self(std::move(io_source));
        }
        return *this;
    }

    //-------------------------------------------------------------------------
    /** @brief ��ԕ����؂�this��������B

        ���ݎ������Ă����ԕ����؂�this��؂藣���A
        �V������ԕ����؂�this��������B

        @param[in,out] io_tree
            �V���Ɏ�����A mosp_tree template�^�̋�ԕ����؁B
        @param[in] in_min �Փ˔���handle�ɑΉ������΍��W�nAABB�̍ŏ��l�B
        @param[in] in_max �Փ˔���handle�ɑΉ������΍��W�nAABB�̍ő�l�B
        @sa detach_tree()
     */
    public: template<typename template_mosp_tree>
    bool attach_tree(
        template_mosp_tree&                               io_tree,
        typename template_mosp_tree::space::vector const& in_min,
        typename template_mosp_tree::space::vector const& in_max)
    {
        // �V����node��p�ӂ���B
        auto const local_node(io_tree.make_node(in_min, in_max));
        if (local_node == nullptr)
        {
            return false;
        }
        local_node->handle_ = this;

        // �Â�node����V����node�֐؂�ւ���B
        if (this->node_ != nullptr)
        {
            PSYQ_ASSERT(this == this->node_->get_handle());
            this->node_->handle_ = nullptr;
        }
        this->node_ = local_node;
        return true;
    }

    /** @brief ��ԕ����؂���this�����O���B

        ���ݎ������Ă����ԕ����؂�this��؂藣���B

        @sa attach_tree()
     */
    public: void detach_tree()
    {
        if (this->node_ != nullptr)
        {
            PSYQ_ASSERT(this == this->node_->get_handle());
            this->node_->handle_ = nullptr;
            this->node_ = nullptr;
        }
    }

    public: mosp_node<template_collision_object> const* get_node() const
    {
        return this->node_;
    }

    //-------------------------------------------------------------------------
    /** @brief this�ɑΉ�����A�Փ˕��̂̎��ʒl�B

        ���̒l�������Ƃ��āA�Փ˔���callback�֐����Ăяo�����B

        @sa mosp_tree::detect_collision()
     */
    public: template_collision_object object_;

    /// this�ɑΉ������ԕ����؂�node�B
    private: mosp_node<template_collision_object>* node_;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief ����class��user���g���̂͋֎~�B��ԕ����؂�node�̌^�B
    @tparam template_collision_object @copydoc mosp_handle::collision_object
 */
template<typename template_collision_object>
class psyq::mosp_node
{
    /// *this�̌^�B
    private: typedef mosp_node<template_collision_object> self;
    friend class mosp_handle<template_collision_object>;

    //-------------------------------------------------------------------------
    /// @brief �P�Ƃ�node���\�z����B
    public: mosp_node()
    :
        handle_(nullptr)
    {
        this->next_ = this;
    }

    /// copy-constructor�͎g�p�֎~�B
    private: mosp_node(self const&);

    /// destructor
    public: ~mosp_node()
    {
        PSYQ_ASSERT(this->is_alone());
        auto const local_handle(this->handle_);
        if (local_handle != nullptr)
        {
            local_handle->detach_tree();
            PSYQ_ASSERT(this->handle_ == nullptr);
        }
    }

    /// copy������Z�q�͎g�p�֎~�B
    private: self& operator=(self const&);

    /** @brief this�ɑΉ�����Փ˔���handle���擾����B
        @retval !=nullptr this�ɑΉ�����Փ˔���handle�ւ̃|�C���^�B
        @retval ==nullptr this�ɑΉ�����Փ˔���handle�����݂��Ȃ��B
     */
    public: mosp_handle<template_collision_object> const* get_handle()
    const
    {
        return this->handle_;
    }

    /** @brief �P�Ƃ�node�����肷��B
     */
    public: bool is_alone() const
    {
        return this == &this->get_next();
    }

    /** @brief ����node���擾����B
        @return ����node�B
     */
    public: const self& get_next() const
    {
        return *this->next_;
    }

    /// @copydoc get_next
    public: self& get_next()
    {
        return *this->next_;
    }

    /** @brief �P�Ƃ�node���Athis�̎��ɑ}������B

        �}������node�́A�P�Ƃł���K�v������B
        io_insert_node������node�ƘA�����Ă���ꍇ�́A�}���Ɏ��s����B

        @param[in,out] io_insert_node �}������node�B
        @retval true  �}���ɐ����B
        @retval false �}���Ɏ��s�����̂ŁA�����s��Ȃ������B
        @sa remove_next is_alone
     */
    public: bool insert_next(
        self& io_insert_node)
    {
        if (!io_insert_node.is_alone())
        {
            // �P�Ƃ̏z�m�[�h�łȂ��Ƒ}���ł��Ȃ��B
            PSYQ_ASSERT(false);
            return false;
        }
        io_insert_node.next_ = &this->get_next();
        this->next_ = &io_insert_node;
        return true;
    }

    /** @brief this�̎��ɂ���node���폜����B

        �폜����node�́A�P�Ƃ�node�ƂȂ�B

        @return �폜����node�B
     */
    public: self& remove_next()
    {
        auto const local_next(&this->get_next());
        this->next_ = &local_next->get_next();
        local_next->next_ = local_next;
        return *local_next;
    }

    //-------------------------------------------------------------------------
    private: self* next_; ///< ����node�B
    /// this�ɑΉ�����Փ˔���handle�B
    private: mosp_handle<template_collision_object>* handle_;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief vector��XYZ������p����morton���W�B

    psyq::mosp_space ��template�����Ɏg���B

    @tparam template_vector @copydoc psyq::mosp_coordinates_xyz::vector
 */
template<typename template_vector>
class psyq::mosp_coordinates_xyz
{
    /// *this�̌^�B
    private: typedef mosp_coordinates_xyz<template_vector> self;

    public: enum: unsigned
    {
        ELEMENT0_INDEX = 0, ///< morton���W�̐���#0��index�ԍ��B
        ELEMENT1_INDEX = 1, ///< morton���W�̐���#1��index�ԍ��B
        ELEMENT2_INDEX = 2, ///< morton���W�̐���#2��index�ԍ��B
    };

    /// morton���W��\��vector�̌^�Bglm::vec3�݊��ł��邱�ƁB
    public: typedef template_vector vector;

    /// morton���W��\��vector�̐����̌^�B
    public: typedef typename template_vector::value_type element;

    /** @brief ��΍��W�n����morton���W�ւ̕ϊ�scale���Z�o����B
        @param[in] in_morton_size morton���W�̍ő�l�B
        @param[in] in_world_size  ��΍��W�n�ł�morton��Ԃ̑傫���B
     */
    public: static typename self::element calc_scale(
        typename self::element const in_morton_size,
        typename self::element const in_world_size)
    {
        if (in_world_size < std::numeric_limits<self::element>::epsilon())
        {
            return 0;
        }
        return in_morton_size / in_world_size;
    }

    /** @brief ��΍��W�n��Ԃ���morton���W��Ԃւ̕ϊ�scale���Z�o����B
        @param[in] in_min   �Փ˔�����s���̈�S�̂��ސ�΍��W�nAABB�̍ŏ��l�B
        @param[in] in_max   �Փ˔�����s���̈�S�̂��ސ�΍��W�nAABB�̍ő�l�B
        @param[in] in_level ��ԕ����̍Ő[level�B
     */
    public: static typename self::vector calc_scale(
        typename self::vector const& in_min,
        typename self::vector const& in_max,
        unsigned const               in_level)
    {
        auto const local_size(in_max - in_min);
        auto const local_unit(
            static_cast<typename self::element>(1 << in_level));
        return self::vector(
            self::calc_scale(local_unit, local_size.operator[](0)),
            self::calc_scale(local_unit, local_size.operator[](1)),
            self::calc_scale(local_unit, local_size.operator[](2)));
    }
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief vector��XZ������p����morton���W�B

    psyq::mosp_space ��template�����Ɏg���B

    @tparam template_vector @copydoc psyq::mosp_coordinates_xyz::vector
 */
template<typename template_vector>
class psyq::mosp_coordinates_xz:
    public psyq::mosp_coordinates_xyz<template_vector>
{
    /// *this�̌^�B
    private: typedef mosp_coordinates_xz<template_vector> self;
    /// *this�̏�ʌ^�B
    public: typedef mosp_coordinates_xyz<template_vector> super;

    public: enum: unsigned
    {
        ELEMENT0_INDEX = 0, ///< morton���W�̐���#0��index�ԍ��B
        ELEMENT1_INDEX = 2, ///< morton���W�̐���#1��index�ԍ��B
    };
    private: enum: unsigned
    {
        ELEMENT2_INDEX = 2, ///< morton���W�̐���#2�͎g�p�ł��Ȃ��B
    };
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief psyq::mosp_tree ��template�����Ɏg���Amorton��Ԃ̊��^�B
    @tparam template_coordinates @copydoc psyq::mosp_space::coordinates
 */
template<typename template_coordinates>
class psyq::mosp_space
{
    /// *this�̌^�B
    private: typedef mosp_space<template_coordinates> self;

    /** @brief �Փ˔���Ɏg��morton���W�̌^�B

        mosp_coordinates_xyz �� mosp_coordinates_xz ���g���B
     */
    public: typedef template_coordinates coordinates;

    /// �Փ˔���Ɏg��vector�̌^�B
    public: typedef typename template_coordinates::vector vector;

    /// morton�����̌^�B
    public: typedef std::uint32_t order;

    /** @brief �Փ˔�����s���̈��ݒ肷��B
        @param[in] in_min   �Փ˔�����s���̈�̐�΍��W�n�ŏ��l�B
        @param[in] in_max   �Փ˔�����s���̈�̐�΍��W�n�ő�l�B
        @param[in] in_level ��ԕ����̍Ő[���x���B
     */
    protected: mosp_space(
        typename self::vector const& in_min,
        typename self::vector const& in_max,
        unsigned const               in_level)
    :
        min_(in_min),
        max_(in_max),
        scale_(self::coordinates::calc_scale(in_min, in_max, in_level))
    {
        PSYQ_ASSERT(this->min_.operator[](0) <= this->max_.operator[](0));
        PSYQ_ASSERT(this->min_.operator[](1) <= this->max_.operator[](1));
        PSYQ_ASSERT(this->min_.operator[](2) <= this->max_.operator[](2));
    }

    protected: typename self::coordinates::element transform_element(
        typename self::vector const& in_vector,
        unsigned const               in_element_index)
    const
    {
        auto local_element(in_vector[in_element_index]);
        auto const local_min(this->min_[in_element_index]);
        if (local_element < local_min)
        {
            return 0;
        }
        auto const local_max(this->max_[in_element_index]);
        if (local_max < local_element)
        {
            local_element = local_max;
        }
        return (local_element - local_min) * this->scale_[in_element_index];
    }

    protected: static typename self::order clamp_axis_order(
        typename self::coordinates::element const in_element,
        typename self::order const                in_max)
    {
        if (in_element < 1)
        {
            return 0;
        }
        const auto local_order(static_cast<typename self::order>(in_element));
        if (local_order < in_max)
        {
            return local_order;
        }
        return in_max;
    }

    /// �Փ˔�����s���̈�́A��΍��W�n�ł̍ŏ����W�B
    private: typename self::vector min_;

    /// �Փ˔�����s���̈�́A��΍��W�n�ł̍ő���W�B
    private: typename self::vector max_;

    /// �ŏ��ƂȂ镪����Ԃ́A��΍��W�n�ł̑傫���̋t���B
    private: typename self::vector scale_;
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief 2�������W�ŏՓ˔�����s��morton��ԁB

    psyq::mosp_tree ��template�����Ɏg���B

    @tparam template_coordinates @copydoc mosp_space::coordinates
 */
template<typename template_coordinates = psyq::mosp_coordinates_xz>
class psyq::mosp_space_2d:
    public psyq::mosp_space<template_coordinates>
{
    /// *this�̌^�B
    private: typedef mosp_space_2d<template_coordinates> self;

    /// *this�̏�ʌ^�B
    public: typedef mosp_space<template_coordinates> super;

    /// �Փ˔���Ɏg�����W�̐����̐��B
    public: static unsigned const DIMENSION = 2;

    /// @copydoc mosp_space::mosp_space
    public: mosp_space_2d(
        typename super::vector const& in_min,
        typename super::vector const& in_max,
        unsigned const                in_level)
    :
        super(in_min, in_max, in_level)
    {
        // pass
    }

    /** @brief 2�������W��̓_����A���`4���؂�morton�������Z�o����B
        @param[in] in_point 2�������W��̓_�B
        @param[in] in_max   morton���W�̍ő�l�B
        @return 2�������W�ɑΉ�����morton�����B
     */
    public: typename super::order calc_order(
        typename super::vector const& in_point,
        typename super::order const   in_max)
    const
    {
        auto const local_element0(
            this->transform_element(
                in_point, super::coordinates::ELEMENT0_INDEX));
        auto const local_element1(
            this->transform_element(
                in_point, super::coordinates::ELEMENT1_INDEX));
        return
            self::separate_bits(local_element0, in_max) << 0 |
            self::separate_bits(local_element1, in_max) << 1;
    }

    /** @brief morton���W���A�����Ƃ�bit�ɕ�������B
        @param[in] in_element morton���W�̐����l�B
        @param[in] in_max     morton���W�̍ő�l�B
     */
    private: static typename super::order separate_bits(
        typename super::coordinates::element const in_element,
        typename super::order const                in_max)
    {
        auto local_bits(super::clamp_axis_order(in_element, in_max));
        local_bits = (local_bits | (local_bits << 8)) & 0x00ff00ff;
        local_bits = (local_bits | (local_bits << 4)) & 0x0f0f0f0f;
        local_bits = (local_bits | (local_bits << 2)) & 0x33333333;
        local_bits = (local_bits | (local_bits << 1)) & 0x55555555;
        return local_bits;
    }
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief 3�������W�ŏՓ˔�����s��moront��ԁB

    mosp_tree ��template�����Ɏg���B

    @tparam template_coordinates @copydoc mosp_space::coordinates
 */
template<typename template_coordinates = psyq::mosp_coordinates_xyz>
class psyq::mosp_space_3d:
    public psyq::mosp_space<template_coordinates>
{
    /// *this�̌^�B
    private: typedef mosp_space_3d<template_coordinates> self;

    /// *this�̏�ʌ^�B
    public: typedef mosp_space<template_coordinates> super;

    /// @copydoc mosp_space_2d::DIMENSION
    public: static unsigned const DIMENSION = 3;

    /// @copydoc mosp_space::mosp_space
    public: mosp_space_3d(
        typename super::vector const& in_min,
        typename super::vector const& in_max,
        unsigned const                in_level)
    :
        super(in_min, in_max, in_level)
    {
        // pass
    }

    /** @brief 3�������W��̓_����A���`8���؂�morton�������Z�o����B
        @param[in] in_point 3�������W��̓_�B
        @param[in] in_max   morton�����̍ő�l�B
        @return 3�������W�ɑΉ�����morton�����B
     */
    public: typename super::order calc_order(
        typename super::vector const& in_point,
        typename self::order const    in_max)
    const
    {
        auto const local_element0(
            this->transform_element(
                in_point, super::coordinates::ELEMENT0_INDEX));
        auto const local_element1(
            this->transform_element(
                in_point, super::coordinates::ELEMENT1_INDEX));
        auto const local_element2(
            this->transform_element(
                in_point, super::coordinates::ELEMENT2_INDEX));
        return
            self::separate_bits(local_element0, in_max) << 0 |
            self::separate_bits(local_element1, in_max) << 1 |
            self::separate_bits(local_element2, in_max) << 2;
    }

    /// @copydoc mosp_space_2d::separate_bits
    private: static typename super::order separate_bits(
        typename super::coordinates::element const in_element,
        typename self::order const                 in_max)
    {
        auto local_bits(super::clamp_axis_order(in_element, in_max));
        local_bits = (local_bits | (local_bits << 8)) & 0x0000f00f;
        local_bits = (local_bits | (local_bits << 4)) & 0x000c30c3;
        local_bits = (local_bits | (local_bits << 2)) & 0x00249249;
        return local_bits;
    }
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief morton�����ɂ���ԕ����؁B

    �g�����̊T�v�B
    -# psyq::mosp_tree::mosp_tree() ���Ăяo���A
       �Փ˔�����s���̈����ԕ����؂ɐݒ肷��B
    -# psyq::mosp_handle::attach_tree() ���Ăяo���A
       �Փ˔���handle����ԕ����؂Ɏ�����B
    -# psyq::mosp_tree::detect_collision() ���Ăяo���A�Փ˂𔻒肷��B

    @tparam template_collision_object @copydoc psyq::mosp_handle::collision_object
    @tparam template_space            @copydoc psyq::mosp_tree::space

    @sa psyq::mosp_handle::attach_tree() psyq::mosp_handle::detach_tree()
 */
template<
    typename template_collision_object,
    typename template_space>
class psyq::mosp_tree
{
    private: typedef mosp_tree self; ///< *this�̌^�B

    /// ��ԕ����؂Ɏ�����A�Փ˔���handle�̌^�B
    public: typedef mosp_handle<template_collision_object> handle;
    /// @cond
    friend handle;
    /// @endcond

    public: typedef mosp_node<template_collision_object> node;

    /** @brief �g�p����morton��Ԃ̌^�B

        mosp_space_2d �� mosp_space_3d ���g���B
     */
    public: typedef template_space space;

    /// ������Ԃ̎����B
    private: typedef std::map<
        typename self::space::order, typename self::node*> cell_map;
    /// node ��memory�����q�B
    private: typedef typename self::cell_map::allocator_type::template
        rebind<typename self::node>::other
            node_allocator;
    /// �Ή��ł����ԕ����̍Ő[���x���B
    public: static const unsigned LEVEL_LIMIT =
        (8 * sizeof(typename self::space::order) - 1) / self::space::DIMENSION;

    //-------------------------------------------------------------------------
    /// @brief �Փ˔���̈�̂Ȃ���ԕ����؂��\�z����B
    public: mosp_tree()
    :
        space_(
            typename self::space::vector(0, 0, 0),
            typename self::space::vector(0, 0, 0),
            0),
        idle_node_(nullptr),
        level_cap_(0),
        detect_collision_(false)
    {
        // pass
    }

    /// copy-constructor�͎g�p�֎~�B
    private: mosp_tree(self const&);

    /** @brief move-constructor�B
        @param[in,out] io_source �ړ����ƂȂ�instance�B
     */
    public: mosp_tree(
        self&& io_source)
    :
        cells_(std::move(io_source.cells_)),
        space_(io_source.space_),
        idle_node_(nullptr),
        level_cap_(io_source.level_cap_),
        detect_collision_(false)
    {
        if (io_source.detect_collision_)
        {
            // �Փ˔��蒆��move�ł��Ȃ��B
            PSYQ_ASSERT(false);
            this->level_cap_ = 0;
        }
        else
        {
            io_source.level_cap_ = 0;
        }
    }

    /// @copydoc mosp_space::mosp_space
    public: mosp_tree(
        typename self::space::vector const& in_min,
        typename self::space::vector const& in_max,
        unsigned const                      in_level = self::LEVEL_LIMIT)
    :
        space_(in_min, in_max, in_level),
        idle_node_(nullptr),
        detect_collision_(false)
    {
        // ��ԕ����̍Ő[level������B
        if (in_level <= self::LEVEL_LIMIT)
        {
            this->level_cap_ = in_level;
        }
        else
        {
            // �ō�level�ȏ�̋�Ԃ͍��Ȃ��B
            PSYQ_ASSERT(false);
            this->level_cap_ = self::LEVEL_LIMIT;
        }
    }

    /** @brief �������Ă���Փ˔���handle�����ׂĎ��O���A
               ��ԕ����؂�j������B
     */
    public: ~mosp_tree()
    {
        // �Փ˔��蒆�́A�j���ł��Ȃ��B
        PSYQ_ASSERT(!this->detect_collision_);
        typename self::node_allocator local_allocator(
            this->cells_.get_allocator());
        self::delete_cell(this->cells_.get_allocator(), this->idle_node_);
        for (auto i(this->cells_.begin()); i != this->cells_.end(); ++i)
        {
            self::delete_cell(this->cells_.get_allocator(), i->second);
        }
    }

    /// copy������Z�q�͎g�p�֎~�B
    private: self& operator=(self const&);

    /** @brief move������Z�q�B
        @param[in,out] io_source �ړ����ƂȂ�instance�B
     */
    public: self& operator=(self&& io_source)
    {
        if (this != &io_source)
        {
            this->~self();
            new(this) self(std::move(io_source));
        }
        return *this;
    }

    //-------------------------------------------------------------------------
    /** @brief ��ԕ����؂�p���āA�Փ˂𔻒肷��B

        mosp_handle::attach_tree() �ɂ���ċ�ԕ����؂Ɏ�����ꂽ
        �Փ˔���n���h���̂����A������Ԃ��d�Ȃ����Փ˔���n���h���̎���
        mosp_handle::collision_object �������Ƃ��āA
        �Փ˔���R�[���o�b�N�֐����Ăяo���B

        @param[in] in_detect_callback
            2�� mosp_handle::collision_object �������Ƃ���A
            �Փ˔���R�[���o�b�N�֐��B
            2�̏Փ˔���n���h���̕�����Ԃ��d�Ȃ����Ƃ��A�Ăяo�����B

        @retval ture �����B�Փ˔�����s�����B
        @retval false
            ���s�B�Փ˔�����s��Ȃ������B
            �����́A���łɏՓ˔�����s�Ȃ��Ă�Œ�������B
     */
    public: template<typename template_detect_callback>
    bool detect_collision(
        template_detect_callback const& in_detect_callback)
    {
        if (!this->detect_collision_)
        {
            this->detect_collision_ = true;
            for (auto i(this->cells_.begin()); i != this->cells_.end();)
            {
                i = this->detect_collision_super_cells(i, in_detect_callback);
            }
            this->detect_collision_ = false;
        }
        return !this->detect_collision_;
    }

    //-------------------------------------------------------------------------
    private: template<typename template_detect_callback>
    typename self::cell_map::iterator detect_collision_super_cells(
        typename self::cell_map::iterator const& in_cell,
        template_detect_callback const&          in_detect_callback)
    {
        // (1) ���̕�����ԂŁA�Փ˔�����s���B
        auto const local_cell(in_cell->second);
        auto local_last_node(local_cell);
        auto local_node(&local_cell->get_next());
        unsigned local_node_count(0);
        while (local_node != local_cell)
        {
            auto const local_exist_handle(
                this->detect_collision_node_and_cell(
                    *local_node, *local_node, *local_cell, in_detect_callback));
            if (local_exist_handle)
            {
                // ����������Ԃ̎���node�ֈڍs����B
                local_last_node = local_node;
                ++local_node_count;
            }
            else
            {
                // ��handle��node�������̂ŁA������Ԃ���폜����B
                this->collect_idle_node(local_last_node->remove_next());
            }
            local_node = &local_last_node->get_next();
        }
        if (&local_cell->get_next() == local_cell)
        {
            return this->cells_.erase(in_cell);
        }

        // (2) ���̕�����ԂƏ�ʂ̕�����ԂŁA�Փ˔�����s���B
        for (auto local_super_order(in_cell->first); 0 < local_super_order;)
        {
            // ��ʂ̕�����ԂɈړ�����B
            local_super_order =
                (local_super_order - 1) >> self::space::DIMENSION;
            auto const local_super_iterator(
                this->cells_.find(local_super_order));
            if (local_super_iterator != this->cells_.end())
            {
                // ��ʂ̕�����ԂƏՓ˔�����s���B
                auto const local_super_cell(local_super_iterator->second);
                this->detect_collision_cell_and_cell(
                    *local_cell, *local_super_cell, in_detect_callback);
                if (&local_cell->get_next() == local_cell)
                {
                    return this->cells_.erase(in_cell);
                }
                if (&local_super_cell->get_next() == local_super_cell)
                {
                    this->cells_.erase(local_super_iterator);
                }
            }
        }
        auto local_iterator(in_cell);
        return ++local_iterator;
    }

    //-------------------------------------------------------------------------
    private: template<typename template_detect_callback>
    void detect_collision_cell_and_cell(
        typename self::node&            io_cell0,
        typename self::node&            io_cell1,
        template_detect_callback const& in_detect_callback)
    {
        auto local_last_node(&io_cell0);
        auto local_node(&io_cell0.get_next());
        while (local_node != &io_cell0)
        {
            auto const local_exist_handle(
                this->detect_collision_node_and_cell(
                    *local_node, io_cell1, io_cell1, in_detect_callback));
            if (local_exist_handle)
            {
                // ����������Ԃ̎���node�ֈڍs����B
                local_last_node = local_node;
            }
            else
            {
                // ��handle��node�������̂ŁA������Ԃ���폜����B
                this->collect_idle_node(local_last_node->remove_next());
            }
            local_node = &local_last_node->get_next();
        }
    }

    /** @brief node�ƕ�����ԂŁA�Փ˔�����s���B
        @param[in,out] io_node             �Փ˔�����s��node�B
        @param[in,out] io_cell_begin_back  �Փ˔�����s��������Ԃ̐擪�̒��O��node�B
        @param[in]     in_cell_end         �Փ˔�����s��������Ԃ̖���node�B
        @param[in]     in_detect_callback  �Փ˔���callback�֐��B
        @retval true  io_node �͋�ɂȂ��ĂȂ��B
        @retval false io_node ����ɂȂ����B
     */
    private: template<typename template_detect_callback>
    bool detect_collision_node_and_cell(
        typename self::node&            io_node,
        typename self::node&            io_cell_begin_back,
        typename self::node const&      in_cell_end,
        template_detect_callback const& in_detect_callback)
    {
        auto local_node_handle(io_node.get_handle());
        if (local_node_handle == nullptr)
        {
            return false;
        }

        auto local_Lastcell(&io_cell_begin_back);
        auto local_cell(&io_cell_begin_back.get_next());
        while (local_cell != &in_cell_end)
        {
            auto const local_cell_handle(local_cell->get_handle());
            if (local_cell_handle != nullptr)
            {
                // �Փ˔���B
                in_detect_callback(
                    local_node_handle->object_, local_cell_handle->object_);

                // �Փ˔���callback�֐��̒��� handle::detach_tree()
                // �����\��������̂ŁA�Ď擾����B
                local_node_handle = io_node.get_handle();
                if (local_node_handle == nullptr)
                {
                    return false;
                }
                local_Lastcell = local_cell;
            }
            else
            {
                // ��handle��node�������̂ŁA������Ԃ���폜����B
                this->collect_idle_node(local_Lastcell->remove_next());
            }
            local_cell = &local_Lastcell->get_next();
        }
        return true;
    }

    //-------------------------------------------------------------------------
    /** @brief AABB���ލŏ��̕�����Ԃ�node���\�z����B
        @param[in] in_min ��΍��W�nAABB�̍ŏ��l�B
        @param[in] in_max ��΍��W�nAABB�̍ő�l�B
        @retval !=nullptr AABB���ލŏ��̕�����Ԃ�node�B
        @retval ==nullptr ���s�B
     */
    private: typename self::node* make_node(
        typename self::space::vector const& in_min,
        typename self::space::vector const& in_max)
    {
        if (this->detect_collision_)
        {
            // detect_collision() �̎��s����node�����Ȃ��B
            PSYQ_ASSERT(false);
            return nullptr;
        }

        // morton�����ɑΉ����镪����Ԃ�p�ӂ���B
        auto const local_morton_order(this->calc_order(in_min, in_max));
        auto& local_cell(this->cells_[local_morton_order]);
        if (local_cell == nullptr)
        {
            local_cell = this->distribute_idle_node();
            if (local_cell == nullptr)
            {
                PSYQ_ASSERT(false);
                this->cells_.erase(local_morton_order);
                return nullptr;
            }
        }

        // morton�����ɑΉ������ԕ�����node��p�ӂ���B
        auto const local_node(this->distribute_idle_node());
        if (local_node == nullptr || !local_cell->insert_next(*local_node))
        {
            PSYQ_ASSERT(local_node != nullptr);
            PSYQ_ASSERT(false);
            return nullptr;
        }
        return local_node;
    }

    //-------------------------------------------------------------------------
    private: typename self::space::order calc_order(
        typename self::space::vector const& in_min,
        typename self::space::vector const& in_max)
    const
    {
        auto const local_level_cap(this->level_cap_);
        if (local_level_cap <= 0)
        {
            return 0;
        }

        // �Փ˕��̂�AABB���ށA�ŏ��̕�����Ԃ�morton�������Z�o����B
        auto const local_axis_order_max((1 << local_level_cap) - 1);
        auto const local_min_morton(
            this->space_.calc_order(in_min, local_axis_order_max));
        auto const local_max_morton(
            this->space_.calc_order(in_max, local_axis_order_max));
        auto const local_morton_distance(local_max_morton ^ local_min_morton);
        auto const local_nlz(psyq::count_leading_bit0(local_morton_distance));
        auto const local_level(
            local_morton_distance != 0?
                (sizeof(local_max_morton) * 8 + 1 - local_nlz)
                    / self::space::DIMENSION:
                1);
        auto const local_num_cells(
            1 << ((local_level_cap - local_level) * self::space::DIMENSION));
        auto const local_base(
            (local_num_cells - 1) / ((1 << self::space::DIMENSION) - 1));
        auto const local_shift(local_level * self::space::DIMENSION);
        // VisualStudio���ƁA32�ȏ��bit-shift���Z�ɕs���������ۂ��B
        PSYQ_ASSERT(local_shift < sizeof(local_max_morton) * 8);
        return local_base + (local_max_morton >> local_shift);
    }

    //-------------------------------------------------------------------------
    /** @brief ��handle��node��z�z����B
        @retval !=nullptr �z�z���ꂽ��handle��node�B
        @retval ==nullptr ���s�B
        @sa collect_idle_node()
     */
    private: typename self::node* distribute_idle_node()
    {
        auto local_node(this->idle_node_);
        if (local_node == nullptr)
        {
            // ��handle��node���Ȃ������̂ŁA�V����node�𐶐�����B
            typename self::node_allocator local_allocator(
                this->cells_.get_allocator());
            return new(local_allocator.allocate(1)) typename self::node;
        }

        // ��handle��node�����o���B
        local_node = &local_node->remove_next();
        if (local_node == this->idle_node_)
        {
            this->idle_node_ = nullptr;
        }
        return local_node;
    }

    /** @brief ��handle��node���������B
        @param[in,out] io_idle_node �������node�B
        @sa distribute_idle_node()
     */
    private: void collect_idle_node(
        typename self::node& io_idle_node)
    {
        if (!io_idle_node.is_alone())
        {
            PSYQ_ASSERT(false);
        }
        else if (
            this->idle_node_ != nullptr &&
            !this->idle_node_->insert_next(io_idle_node))
        {
            PSYQ_ASSERT(false);
        }
        else
        {
            this->idle_node_ = &io_idle_node;
        }
    }

    private: static void delete_cell(
        typename self::node_allocator io_allocator,
        typename self::node* const    io_cell)
    {
        if (io_cell != nullptr)
        {
            for (;;)
            {
                auto const local_cell(&io_cell->remove_next());
                local_cell->~node();
                io_allocator.deallocate(local_cell, 1);
                if (io_cell == local_cell)
                {
                    break;
                }
            }
        }
    }

    //-------------------------------------------------------------------------
    private: typename self::cell_map cells_; ///< ������Ԃ̎����B
    private: typename self::space space_; ///< �g�p����morton��ԁB
    private: typename self::node* idle_node_; ///< ��handle��node�B
    private: std::size_t level_cap_; ///< ��ԕ����̍Ő[level�B
    private: bool detect_collision_; ///< detect_collision() �����s�����ǂ����B
};

#endif // !PSYQ_MOSP_COLLISION_HPP_
