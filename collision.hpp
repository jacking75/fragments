#ifndef PSYQ_COLLISION_HPP_
#define PSYQ_COLLISION_HPP_

#include <glm/glm.hpp>

namespace psyq_collision
{
/// �Փ˔���Ɏg��������
struct half_line
{
    glm::vec3 origin;    ///< �������̊J�n�ʒu
    glm::vec3 direction; ///< �������̕����B������1�B
    float     length;    ///< �������̒����B
};

/// �������Ƃ̏Փ˔���Ɏg���O�p�`�B
struct triangle
{
    /** @brief �������B
        @param[in] in_vertex0 �O�p�`�̒��_#0
        @param[in] in_vertex1 �O�p�`�̒��_#1
        @param[in] in_vertex2 �O�p�`�̒��_#2
        @param[in] in_epsilon �v�Z�@epsilon�l�B
        @retval true  �����B
        @retval false ���s�B
     */
    bool initialize(
        glm::vec3 const& in_vertex0,
        glm::vec3 const& in_vertex1,
        glm::vec3 const& in_vertex2,
        float const      in_epsilon =
            std::numeric_limits<float>::epsilon())
    {
        glm::vec3 local_edge1(in_vertex1 - in_vertex0);
        glm::vec3 local_edge2(in_vertex2 - in_vertex0);
        this->normal = glm::cross(local_edge1, local_edge2);
        glm::vec3 const local_nx(glm::cross(local_edge2, this->normal));
        glm::vec3 const local_ny(glm::cross(local_edge1, this->normal));
        this->binormal_u = local_nx / glm::dot(local_edge1, local_nx);
        this->binormal_v = local_ny / glm::dot(local_edge2, local_ny);
        return true;
        /*
        glm::vec3 const local_e2(glm::cross(in_vertex0, in_vertex1));
        float const local_d(glm::dot(local_e2, in_vertex2));
        if (local_d < in_epsilon)
        {
            return false;
        }
        this->origin = in_vertex0;
        this->normal = glm::cross(
            in_vertex1 - in_vertex0, in_vertex2 - in_vertex0);
        float const local_invert_d(1 / local_d);
        glm::vec3 const local_e1(glm::cross(in_vertex2, in_vertex0));
        this->binormal_u = local_e1 * local_invert_d;
        this->binormal_v = local_e2 * local_invert_d;
        return true;
         */
    }

    /** @brief �������Ƃ̏Փ˔���B

        �ȉ��� web page ���Q�l�ɂ��܂����B
        http://d.hatena.ne.jp/ototoi/20050320/p1

        �Փ˂����ꍇ�A�Փˈʒu�͈ȉ��̎��ŎZ�o�ł���B
        in_half_line.origin + in_half_line.direction * out_intersection.x

        �܂����_���ɖ@��������ꍇ�A�ȉ��̎��ŏՓˈʒu�̖@�����Z�o�ł���B
        normal0 * (1 - out_intersection.y - out_intersection.z) +
        normal1 * out_intersection.y +
        normal2 * out_intersection.z

        @param[out] out_intersection �Փˎ���[t,u,v]�l�B
        @param[in]  in_ray_origin    ���肷�锼�����̎n�_�B
        @param[in]  in_ray_direction �������̕����B������1�B
        @param[in]  in_ray_length    �������̒����B
        @param[in]  in_epsilon       �v�Z�@epsilon�l�B
        @retval true  �������ƏՓ˂����B
        @retval false �������ƏՓ˂��Ȃ������B
     */
    bool intersect(
        glm::vec3&       out_intersection,
        glm::vec3 const& in_ray_origin,
        glm::vec3 const& in_ray_direction,
        float const      in_ray_length,
        float const      in_epsilon = 0)
    const
    {
        float const local_nv(-glm::dot(in_ray_direction, this->normal));
        if (local_nv < in_epsilon)
        {
            return false;
        }

        glm::vec3 const local_origin_diff(in_ray_origin - this->origin);
        float const local_t(
            glm::dot(local_origin_diff, this->normal) / local_nv);
        if (local_t < 0 || in_ray_length < local_t)
        {
            return false;
        }

        glm::vec3 const local_position(
            in_ray_direction * local_t + local_origin_diff);
        float const local_u(glm::dot(local_position, this->binormal_u));
        if (local_u < in_epsilon)
        {
            return false;
        }
        float const local_v(glm::dot(local_position, this->binormal_v));
        if (local_v < in_epsilon || 1 - in_epsilon < local_u + local_v)
        {
            return false;
        }

        out_intersection = glm::vec3(local_t, local_u, local_v);
        return true;
    }

    glm::vec3 origin;     ///< �O�p�`�̊J�n�ʒu
    glm::vec3 normal;     ///< �O�p�`�̖@��
    glm::vec3 binormal_u; ///< �O�p�`�̏d�S���WU
    glm::vec3 binormal_v; ///< �O�p�`�̏d�S���WV 
};

} // namespace psyq_collision

#endif // PSYQ_COLLISION_HPP_
