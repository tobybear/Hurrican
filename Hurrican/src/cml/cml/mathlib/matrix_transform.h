/* -*- C++ -*- ------------------------------------------------------------

Copyright (c) 2007 Jesse Anders and Demian Nave http://cmldev.net/

The Configurable Math Library (CML) is distributed under the terms of the
Boost Software License, v1.0 (see cml/LICENSE for details).

 *-----------------------------------------------------------------------*/
/** @file
 *  @brief
 */

#ifndef matrix_transform_h
#define matrix_transform_h

#include <cml/mathlib/matrix_basis.h>
#include <cml/mathlib/matrix_rotation.h>
#include <cml/mathlib/matrix_translation.h>

/* Functions for building matrix transforms other than rotations
 * (matrix_rotation.h) and viewing projections (matrix_projection.h).
 */

namespace cml
{

//////////////////////////////////////////////////////////////////////////////
// 3D translation
//////////////////////////////////////////////////////////////////////////////

/** Build a matrix representing a 3D translation */
template < typename E, class A, class B, class L > void
matrix_translation(matrix<E,A,B,L>& m, E x, E y, E z)
{
    identity_transform(m);
    matrix_set_translation(m,x,y,z);
}

/** Build a matrix representing a 3D translation with z set to 0 */
template < typename E, class A, class B, class L > void
matrix_translation(matrix<E,A,B,L>& m, E x, E y)
{
    identity_transform(m);
    matrix_set_translation(m,x,y);
}

/** Build a matrix representing a 3D translation */
template < typename E, class A, class B, class L, class VecT > void
matrix_translation(matrix<E,A,B,L>& m, const VecT& translation)
{
    identity_transform(m);
    matrix_set_translation(m,translation);
}

//////////////////////////////////////////////////////////////////////////////
// 2D translation
//////////////////////////////////////////////////////////////////////////////

/** Build a matrix representing a 2D translation */
template < typename E, class A, class B, class L > void
matrix_translation_2D(matrix<E,A,B,L>& m, E x, E y)
{
    identity_transform(m);
    matrix_set_translation_2D(m,x,y);
}

/** Build a matrix representing a 2D translation */
template < typename E, class A, class B, class L, class VecT > void
matrix_translation_2D(matrix<E,A,B,L>& m, const VecT& translation)
{
    identity_transform(m);
    matrix_set_translation_2D(m, translation);
}

//////////////////////////////////////////////////////////////////////////////
// 3D scale
//////////////////////////////////////////////////////////////////////////////

/** Build a matrix representing a uniform 3D scale */
template < typename E, class A, class B, class L > void
matrix_uniform_scale(matrix<E,A,B,L>& m, E scale)
{
    matrix_scale(m,scale,scale,scale);
}

/** Build a matrix representing a non-uniform 3D scale */
template < typename E, class A, class B, class L > void
matrix_scale(matrix<E,A,B,L>& m, E scale_x, E scale_y, E scale_z)
{
    /* Checking */
    detail::CheckMatLinear3D(m);

    identity_transform(m);

    m.set_basis_element(0,0,scale_x);
    m.set_basis_element(1,1,scale_y);
    m.set_basis_element(2,2,scale_z);
}

/** Build a matrix representing a non-uniform 3D scale */
template < typename E, class A, class B, class L, class VecT > void
matrix_scale(matrix<E,A,B,L>& m, const VecT& scale)
{
    /* Checking */
    detail::CheckVec3(scale);

    matrix_scale(m, scale[0], scale[1], scale[2]);
}

//////////////////////////////////////////////////////////////////////////////
// 2D scale
//////////////////////////////////////////////////////////////////////////////

/** Build a matrix representing a uniform 2D scale */
template < typename E, class A, class B, class L > void
matrix_uniform_scale_2D(matrix<E,A,B,L>& m, E scale)
{
    matrix_scale_2D(m,scale,scale);
}

/** Build a matrix representing a non-uniform 2D scale */
template < typename E, class A, class B, class L > void
matrix_scale_2D(matrix<E,A,B,L>& m, E scale_x, E scale_y)
{
    /* Checking */
    detail::CheckMatLinear2D(m);

    identity_transform(m);

    m.set_basis_element(0,0,scale_x);
    m.set_basis_element(1,1,scale_y);
}

/** Build a matrix representing a non-uniform 2D scale */
template < typename E, class A, class B, class L, class VecT > void
matrix_scale_2D(matrix<E,A,B,L>& m, const VecT& scale)
{
    /* Checking */
    detail::CheckVec2(scale);

    matrix_scale_2D(m, scale[0], scale[1]);
}

//////////////////////////////////////////////////////////////////////////////
// 3D scale along axis
//////////////////////////////////////////////////////////////////////////////

/** Build a matrix representing a 3D scale along an arbitrary axis */
template < typename E, class A, class B, class L, class VecT > void
matrix_scale_along_axis(matrix<E,A,B,L>&m, const VecT& axis, E scale)
{
    typedef matrix<E,A,B,L> matrix_type;
    [[maybe_unused]] typedef typename matrix_type::value_type value_type;

    /* Checking */
    detail::CheckVec3(axis);

    matrix<E,fixed<3,3>,B,L> outer_p = outer(axis,axis)*(scale-value_type(1));
    outer_p(0,0) += value_type(1);
    outer_p(1,1) += value_type(1);
    outer_p(2,2) += value_type(1);

    matrix_linear_transform(m, outer_p);
}

//////////////////////////////////////////////////////////////////////////////
// 2D scale along axis
//////////////////////////////////////////////////////////////////////////////

/** Build a matrix representing a 2D scale along an arbitrary axis */
template < typename E, class A, class B, class L, class VecT >
void matrix_scale_along_axis_2D(matrix<E,A,B,L>&  m, const VecT& axis,
                                E scale)
{
    typedef matrix<E,A,B,L> matrix_type;
    [[maybe_unused]] typedef typename matrix_type::value_type value_type;

    /* Checking */
    detail::CheckVec2(axis);

    matrix<E,fixed<2,2>,B,L> outer_p = outer(axis,axis)*(scale-value_type(1));
    outer_p(0,0) += value_type(1);
    outer_p(1,1) += value_type(1);

    matrix_linear_transform_2D(m, outer_p);
}

//////////////////////////////////////////////////////////////////////////////
// 3D shear
//////////////////////////////////////////////////////////////////////////////

/** Build a matrix representing a 3D shear along the specified world axis */
template < typename E, class A, class B, class L > void
matrix_shear(matrix<E,A,B,L>& m, size_t axis, E shear_s, E shear_t)
{
    /* Checking */
    detail::CheckMatLinear3D(m);
    detail::CheckIndex3(axis);

    identity_transform(m);

    size_t i, j, k;
    cyclic_permutation(axis, i, j, k);

    m.set_basis_element(i,j,shear_s);
    m.set_basis_element(i,k,shear_t);
}

/** Build a matrix representing a 3D shear along the world x axis */
template < typename E, class A, class B, class L > void
matrix_shear_x(matrix<E,A,B,L>& m, E shear_s, E shear_t)
{
    matrix_shear(m,0,shear_s,shear_t);
}

/** Build a matrix representing a 3D shear along the world y axis */
template < typename E, class A, class B, class L > void
matrix_shear_y(matrix<E,A,B,L>& m, E shear_s, E shear_t)
{
    matrix_shear(m,1,shear_s,shear_t);
}

/** Build a matrix representing a 3D shear along the world z axis */
template < typename E, class A, class B, class L > void
matrix_shear_z(matrix<E,A,B,L>& m, E shear_s, E shear_t)
{
    matrix_shear(m,2,shear_s,shear_t);
}

//////////////////////////////////////////////////////////////////////////////
// 2D shear
//////////////////////////////////////////////////////////////////////////////

/** Build a matrix representing a 2D shear along the specified world axis */
template < typename E, class A, class B, class L > void
matrix_shear_2D(matrix<E,A,B,L>& m, size_t axis, E shear)
{
    /* Checking */
    detail::CheckMatLinear2D(m);
    detail::CheckIndex2(axis);

    identity_transform(m);

    size_t i, j;
    cyclic_permutation(axis, i, j);

    m.set_basis_element(i,j,shear);
}

/** Build a matrix representing a 2D shear along the world x axis */
template < typename E, class A, class B, class L > void
matrix_shear_x_2D(matrix<E,A,B,L>& m, E shear)
{
    matrix_shear_2D(m,0,shear);
}

/** Build a matrix representing a 2D shear along the world y axis */
template < typename E, class A, class B, class L > void
matrix_shear_y_2D(matrix<E,A,B,L>& m, E shear)
{
    matrix_shear_2D(m,1,shear);
}

//////////////////////////////////////////////////////////////////////////////
// 3D reflection
//////////////////////////////////////////////////////////////////////////////

/** Build a matrix representing a 3D reflection along the given world axis */
template < typename E, class A, class B, class L > void
matrix_reflect(matrix<E,A,B,L>& m, size_t axis)
{
    typedef matrix<E,A,B,L> matrix_type;
    [[maybe_unused]] typedef typename matrix_type::value_type value_type;

    /* Checking */
    detail::CheckMatLinear3D(m);
    detail::CheckIndex3(axis);

    identity_transform(m);

    m(axis,axis) = value_type(-1);
}

/** Build a matrix representing a 3D reflection along the world x axis */
template < typename E, class A, class B, class L > void
matrix_reflect_x(matrix<E,A,B,L>& m)
{
    matrix_reflect(m,0);
}

/** Build a matrix representing a 3D reflection along the world y axis */
template < typename E, class A, class B, class L > void
matrix_reflect_y(matrix<E,A,B,L>& m)
{
    matrix_reflect(m,1);
}

/** Build a matrix representing a 3D reflection along the world z axis */
template < typename E, class A, class B, class L > void
matrix_reflect_z(matrix<E,A,B,L>& m)
{
    matrix_reflect(m,2);
}

//////////////////////////////////////////////////////////////////////////////
// 2D reflection
//////////////////////////////////////////////////////////////////////////////

/** Build a matrix representing a 2D reflection along the given world axis */
template < typename E, class A, class B, class L > void
matrix_reflect_2D(matrix<E,A,B,L>& m, size_t axis)
{
    typedef matrix<E,A,B,L> matrix_type;
    [[maybe_unused]] typedef typename matrix_type::value_type value_type;

    /* Checking */
    detail::CheckMatLinear2D(m);
    detail::CheckIndex2(axis);

    identity_transform(m);

    m(axis,axis) = value_type(-1);
}

/** Build a matrix representing a 2D reflection along the world x axis */
template < typename E, class A, class B, class L > void
matrix_reflect_x_2D(matrix<E,A,B,L>& m)
{
    matrix_reflect_2D(m,0);
}

/** Build a matrix representing a 2D reflection along the world y axis */
template < typename E, class A, class B, class L > void
matrix_reflect_y_2D(matrix<E,A,B,L>& m)
{
    matrix_reflect_2D(m,1);
}

//////////////////////////////////////////////////////////////////////////////
// 3D reflection about hyperplane
//////////////////////////////////////////////////////////////////////////////

/** Build a matrix representing a 3D reflection about the given hyperplane */
template < typename E, class A, class B, class L, class VecT > void
matrix_reflect_about_hplane(matrix<E,A,B,L>& m, const VecT& normal)
{
    typedef matrix<E,A,B,L> matrix_type;
    [[maybe_unused]] typedef typename matrix_type::value_type value_type;

    matrix_scale_along_axis(m, normal, value_type(-1));
}

//////////////////////////////////////////////////////////////////////////////
// 2D reflection about hyperplane
//////////////////////////////////////////////////////////////////////////////

/** Build a matrix representing a 2D reflection about the given hyperplane */
template < typename E, class A, class B, class L, class VecT > void
matrix_reflect_about_hplane_2D(matrix<E,A,B,L>&m, const VecT& normal)
{
    typedef matrix<E,A,B,L> matrix_type;
    [[maybe_unused]] typedef typename matrix_type::value_type value_type;

    matrix_scale_along_axis_2D(m, normal, value_type(-1));
}

//////////////////////////////////////////////////////////////////////////////
// 3D orthographic projection to cardinal hyperplane
//////////////////////////////////////////////////////////////////////////////

/** Build a matrix representing an orthographic projection onto a plane */
template < typename E, class A, class B, class L > void
matrix_ortho_project(matrix<E,A,B,L>& m, size_t axis)
{
    typedef matrix<E,A,B,L> matrix_type;
    [[maybe_unused]] typedef typename matrix_type::value_type value_type;

    /* Checking */
    detail::CheckMatLinear3D(m);
    detail::CheckIndex3(axis);

    identity_transform(m);

    m(axis,axis) = value_type(0);
}

/** Build a matrix representing an orthographic projection onto the yz plane*/
template < typename E, class A, class B, class L > void
matrix_ortho_project_yz(matrix<E,A,B,L>& m)
{
    matrix_ortho_project(m,0);
}

/** Build a matrix representing an orthographic projection onto the zx plane*/
template < typename E, class A, class B, class L > void
matrix_ortho_project_zx(matrix<E,A,B,L>& m)
{
    matrix_ortho_project(m,1);
}

/** Build a matrix representing an orthographic projection onto the zy plane*/
template < typename E, class A, class B, class L > void
matrix_ortho_project_xy(matrix<E,A,B,L>& m)
{
    matrix_ortho_project(m,2);
}

//////////////////////////////////////////////////////////////////////////////
// 2D orthographic projection to cardinal hyperplane
//////////////////////////////////////////////////////////////////////////////

/** Build a matrix representing a 2D orthographic projection */
template < typename E, class A, class B, class L > void
matrix_ortho_project_2D(matrix<E,A,B,L>& m, size_t axis)
{
    typedef matrix<E,A,B,L> matrix_type;
    [[maybe_unused]] typedef typename matrix_type::value_type value_type;

    /* Checking */
    detail::CheckMatLinear2D(m);
    detail::CheckIndex2(axis);

    identity_transform(m);

    m(axis,axis) = value_type(0);
}

/** Build a matrix representing an orthographic projection onto the y axis */
template < typename E, class A, class B, class L > void
matrix_ortho_project_y_2D(matrix<E,A,B,L>& m)
{
    matrix_ortho_project_2D(m,0);
}

/** Build a matrix representing an orthographic projection onto the x axis */
template < typename E, class A, class B, class L > void
matrix_ortho_project_x_2D(matrix<E,A,B,L>& m)
{
    matrix_ortho_project_2D(m,1);
}

//////////////////////////////////////////////////////////////////////////////
// 3D orthographic projection to hyperplane
//////////////////////////////////////////////////////////////////////////////

/** Build a matrix representing a 3D orthographic projection about the given
 * hyperplane passing through the origin.
 */
template < typename E, class A, class B, class L, class VecT > void
matrix_ortho_project_to_hplane(matrix<E,A,B,L>& m, const VecT& normal)
{
    typedef matrix<E,A,B,L> matrix_type;
    [[maybe_unused]] typedef typename matrix_type::value_type value_type;

    matrix_scale_along_axis(m, normal, value_type(0));
}

//////////////////////////////////////////////////////////////////////////////
// 2D orthographic projection to hyperplane
//////////////////////////////////////////////////////////////////////////////

/** Build a matrix representing a 2D orthographic projection about the given
 * hyperplane passing through the origin.
 */
template < typename E, class A, class B, class L, class VecT > void
matrix_ortho_project_to_hplane_2D(matrix<E,A,B,L>& m, const VecT& normal)
{
    typedef matrix<E,A,B,L> matrix_type;
    [[maybe_unused]] typedef typename matrix_type::value_type value_type;

    matrix_scale_along_axis_2D(m, normal, value_type(0));
}

//////////////////////////////////////////////////////////////////////////////
// 3D 'aim at'
//////////////////////////////////////////////////////////////////////////////

/** See vector_ortho.h for details */
template < typename E, class A, class B, class L,
           class VecT_1, class VecT_2, class VecT_3 > void
matrix_aim_at(matrix<E,A,B,L>& m, const VecT_1& pos, const VecT_2& target,
              const VecT_3& reference,
              AxisOrder order = axis_order_zyx)
{
    matrix_rotation_aim_at(m, pos, target, reference, order);
    matrix_set_translation(m, pos);
}

/** See vector_ortho.h for details */
template < typename E, class A, class B, class L,
           class VecT_1, class VecT_2 > void
matrix_aim_at(matrix<E,A,B,L>& m, const VecT_1& pos, const VecT_2& target,
              AxisOrder order = axis_order_zyx)
{
    matrix_rotation_aim_at(m, pos, target, order);
    matrix_set_translation(m, pos);
}

/** See vector_ortho.h for details */
template < typename E, class A, class B, class L,
           class VecT_1, class VecT_2, class VecT_3 > void
matrix_aim_at_axial(
    matrix<E,A,B,L>& m,
    const VecT_1& pos,
    const VecT_2& target,
    const VecT_3& axis,
    AxisOrder order = axis_order_zyx)
{
    matrix_rotation_aim_at_axial(m, pos, target, axis, order);
    matrix_set_translation(m, pos);
}

/** See vector_ortho.h for details */
template < typename E,class A,class B,class L,class VecT,class MatT > void
matrix_aim_at_viewplane(
    matrix<E,A,B,L>& m,
    const VecT& pos,
    const MatT& view_matrix,
    Handedness handedness,
    AxisOrder order = axis_order_zyx)
{
    matrix_rotation_align_viewplane(m, view_matrix, handedness, order);
    matrix_set_translation(m, pos);
}

//////////////////////////////////////////////////////////////////////////////
// 2D 'aim at'
//////////////////////////////////////////////////////////////////////////////

/** See vector_ortho.h for details */
template < typename E,class A,class B,class L,class VecT_1,class VecT_2 > void
matrix_aim_at_2D(
    matrix<E,A,B,L>& m,
    const VecT_1& pos,
    const VecT_2& target,
    AxisOrder2D order = axis_order_xy)
{
    matrix_rotation_align_2D(m, target - pos, true, order);
    matrix_set_translation_2D(m, pos);
}

//////////////////////////////////////////////////////////////////////////////
// 3D 'look at' view matrix
//////////////////////////////////////////////////////////////////////////////

/** Build a matrix representing a 'look at' view transform */
template < typename E, class A, class B, class L,
           class VecT_1, class VecT_2, class VecT_3 > void
matrix_look_at(
    matrix<E,A,B,L>& m,
    const VecT_1& eye,
    const VecT_2& target,
    const VecT_3& up,
    Handedness handedness)
{
    typedef matrix<E,A,B,L> matrix_type;
    [[maybe_unused]] typedef vector< E,fixed<3> > vector_type;
    [[maybe_unused]] typedef typename matrix_type::value_type value_type;

    /* Checking */
    detail::CheckMatAffine3D(m);

    identity_transform(m);

    value_type s = handedness == left_handed ? 1 : -1;
    vector_type z = s * normalize(target - eye);
    vector_type x = unit_cross(up,z);
    vector_type y = cross(z,x);

    matrix_set_transposed_basis_vectors(m,x,y,z);
    matrix_set_translation(m,-dot(eye,x),-dot(eye,y),-dot(eye,z));
}

/** Build a matrix representing a left-handedness 'look at' view transform */
template < typename E, class A, class B, class L,
           class VecT_1, class VecT_2, class VecT_3 > void
matrix_look_at_LH(matrix<E,A,B,L>& m, const VecT_1& eye,
                  const VecT_2& target, const VecT_3& up)
{
    matrix_look_at(m, eye, target, up, left_handed);
}

/** Build a matrix representing a right-handedness 'look at' view transform */
template < typename E, class A, class B, class L,
           class VecT_1, class VecT_2, class VecT_3 > void
matrix_look_at_RH(matrix<E,A,B,L>& m, const VecT_1& eye,
                  const VecT_2& target, const VecT_3& up)
{
    matrix_look_at(m, eye, target, up, right_handed);
}

/** Build a matrix representing a 'look at' view transform */
template < typename E, class A, class B, class L > void
matrix_look_at(matrix<E,A,B,L>& m, E eye_x, E eye_y, E eye_z, E target_x,
               E target_y, E target_z, E up_x, E up_y, E up_z,
               Handedness handedness)
{
    [[maybe_unused]] typedef vector< E, fixed<3> > vector_type;

    matrix_look_at(m,
                   vector_type(eye_x,eye_y,eye_z),
                   vector_type(target_x,target_y,target_z),
                   vector_type(up_x,up_y,up_z),
                   handedness
                  );
}

/** Build a matrix representing a left-handed'look at' view transform */
template < typename E, class A, class B, class L > void
matrix_look_at_LH(matrix<E,A,B,L>& m, E eye_x, E eye_y, E eye_z,
                  E target_x, E target_y, E target_z, E up_x, E up_y, E up_z)
{
    matrix_look_at(m,eye_x,eye_y,eye_z,target_x,target_y,target_z,up_x,up_y,
                   up_z,left_handed);
}

/** Build a matrix representing a right-handed'look at' view transform */
template < typename E, class A, class B, class L > void
matrix_look_at_RH(matrix<E,A,B,L>& m, E eye_x, E eye_y, E eye_z,
                  E target_x, E target_y, E target_z, E up_x, E up_y, E up_z)
{
    matrix_look_at(m,eye_x,eye_y,eye_z,target_x,target_y,target_z,up_x,up_y,
                   up_z,right_handed);
}

//////////////////////////////////////////////////////////////////////////////
// 3D linear transform
//////////////////////////////////////////////////////////////////////////////

/** Build a matrix from the 3x3 linear transform part of another matrix */
template < typename E, class A, class B, class L, class MatT > void
matrix_linear_transform(matrix<E,A,B,L>& m, const MatT& linear)
{
    /* Checking */
    detail::CheckMatLinear3D(m);
    detail::CheckMatLinear3D(linear);

    identity_transform(m);

    for(size_t i = 0; i < 3; ++i)
    {
        for(size_t j = 0; j < 3; ++j)
        {
            m.set_basis_element(i,j,linear.basis_element(i,j));
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
// 2D linear transform
//////////////////////////////////////////////////////////////////////////////

/** Build a matrix from the 2x2 linear transform part of another matrix */
template < typename E, class A, class B, class L, class MatT > void
matrix_linear_transform_2D(matrix<E,A,B,L>& m, const MatT& linear)
{
    /* Checking */
    detail::CheckMatLinear2D(m);
    detail::CheckMatLinear2D(linear);

    identity_transform(m);

    for(size_t i = 0; i < 2; ++i)
    {
        for(size_t j = 0; j < 2; ++j)
        {
            m.set_basis_element(i,j,linear.basis_element(i,j));
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
// 3D affine transform
//////////////////////////////////////////////////////////////////////////////

/** 3D affine transform from three basis vectors and a translation */
template <typename E, class A, class B, class L,
          class VecT_1, class VecT_2, class VecT_3, class VecT_4 > void
matrix_affine_transform(matrix<E,A,B,L>& m, const VecT_1& x, const VecT_2& y,
                        const VecT_3& z, const VecT_4& translation)
{
    identity_transform(m);
    matrix_set_basis_vectors(m,x,y,z);
    matrix_set_translation(m,translation);
}

/** 3D affine transform from a quaternion and a translation */
template <
    typename E, class A, class B, class L,
    typename QE, class QA, class O, class C, class VecT > void
matrix_affine_transform(
    matrix<E,A,B,L>& m, const quaternion<QE,QA,O,C>& q,
    const VecT& translation)
{
    matrix_rotation_quaternion(m,q);
    matrix_set_translation(m,translation);
}

/** 3D affine transform from a quaternion expression and a translation */
template < typename E,class A,class B,class L,class XprT,class VecT > void
matrix_affine_transform(
    matrix<E,A,B,L>& m, const et::QuaternionXpr<XprT>& q,
    const VecT& translation)
{
    matrix_rotation_quaternion(m,q);
    matrix_set_translation(m,translation);
}

/** 3D affine transform from an axis-angle pair and a translation */
template <
    typename E, class A, class B, class L, class VecT_1, class VecT_2 > void
matrix_affine_transform(
    matrix<E,A,B,L>& m,const VecT_1& axis,E angle,const VecT_2& translation)
{
    matrix_rotation_axis_angle(m,axis,angle);
    matrix_set_translation(m,translation);
}

/** 3D affine transform from an Euler-angle triple and a translation */
template < typename E, class A, class B, class L, class VecT > void
matrix_affine_transform(matrix<E,A,B,L>& m, E angle_0, E angle_1,
                        E angle_2, EulerOrder order, const VecT& translation)
{
    matrix_rotation_euler(m,angle_0,angle_1,angle_2,order);
    matrix_set_translation(m,translation);
}

/** 3D affine transform from a matrix and a translation */
template <
    typename E, class A, class B, class L,
    typename ME, class MA, class MB, class ML, class VecT > void
matrix_affine_transform(matrix<E,A,B,L>& m,
                        const matrix<ME,MA,MB,ML>& linear, const VecT& translation)
{
    matrix_linear_transform(m,linear);
    matrix_set_translation(m,translation);
}

/** 3D affine transform from a matrix expression and a translation */
template < typename E,class A,class B,class L,class XprT,class VecT > void
matrix_affine_transform(
    matrix<E,A,B,L>& m, const et::MatrixXpr<XprT>& linear,
    const VecT& translation)
{
    matrix_linear_transform(m,linear);
    matrix_set_translation(m,translation);
}

//////////////////////////////////////////////////////////////////////////////
// 2D affine transform
//////////////////////////////////////////////////////////////////////////////

/** 2D affine transform from two basis vectors and a translation */
template <typename E, class A, class B, class L,
          class VecT_1, class VecT_2, class VecT_3 > void
matrix_affine_transform_2D(matrix<E,A,B,L>& m, const VecT_1& x,
                           const VecT_2& y, const VecT_3& translation)
{
    identity_transform(m);
    matrix_set_basis_vectors_2D(m,x,y);
    matrix_set_translation_2D(m,translation);
}

/** 2D affine transform from a rotation angle and a translation */
template <typename E, class A, class B, class L, class VecT >
void matrix_affine_transform_2D(matrix<E,A,B,L>& m, E angle,
                                const VecT& translation)
{
    matrix_rotation_2D(m,angle);
    matrix_set_translation_2D(m,translation);
}

/** 2D affine transform from a matrix and a translation */
template < typename E,class A,class B,class L,class MatT,class VecT > void
matrix_affine_transform_2D(
    matrix<E,A,B,L>& m, const MatT& linear, const VecT& translation)
{
    matrix_linear_transform_2D(m, linear);
    matrix_set_translation_2D(m,translation);
}

//////////////////////////////////////////////////////////////////////////////
// 3D affine from 2D affine
//////////////////////////////////////////////////////////////////////////////

/** Construct a 3D affine transform from a 2D affine transform */
template < typename E, class A, class B, class L, class MatT > void
matrix_3D_affine_from_2D_affine(matrix<E,A,B,L>& m, const MatT& affine_2D)
{
    [[maybe_unused]] typedef vector< E, fixed<2> > vector_type;

    vector_type x = matrix_get_x_basis_vector_2D(affine_2D);
    vector_type y = matrix_get_y_basis_vector_2D(affine_2D);
    vector_type p = matrix_get_translation_2D(affine_2D);

    identity_transform(m);

    matrix_set_basis_vectors_2D(m,x,y);
    matrix_set_translation(m,p);
}

//////////////////////////////////////////////////////////////////////////////
// 3D affine from 3D affine
//////////////////////////////////////////////////////////////////////////////

/** Construct a 3D affine transform from another 3D affine transform */
template < typename E, class A, class B, class L, class MatT > void
matrix_3D_affine_from_3D_affine(matrix<E,A,B,L>& m, const MatT& affine_3D)
{
    [[maybe_unused]] typedef vector< E, fixed<3> > vector_type;

    vector_type x = matrix_get_x_basis_vector(affine_3D);
    vector_type y = matrix_get_y_basis_vector(affine_3D);
    vector_type z = matrix_get_z_basis_vector(affine_3D);
    vector_type p = matrix_get_translation(affine_3D);

    identity_transform(m);

    matrix_set_basis_vectors(m,x,y,z);
    matrix_set_translation(m,p);
}

//////////////////////////////////////////////////////////////////////////////
// Matrix decomposition (scale->rotate->translate)
//////////////////////////////////////////////////////////////////////////////

/* 3x3 matrix version */
template <
    class MatT,
    typename Real,
    typename ME,
    class MA,
    class B,
    class L,
    typename VE,
    class VA
    >
void matrix_decompose_SRT(
    const MatT& m,
    Real& scale_x,
    Real& scale_y,
    Real& scale_z,
    matrix<ME,MA,B,L>& rotation,
    vector<VE,VA>& translation)
{
    typedef MatT matrix_type;
    [[maybe_unused]] typedef typename matrix_type::value_type value_type;
    [[maybe_unused]] typedef vector<value_type, fixed<3> > vector_type;

    /* Checking */
    detail::CheckMatAffine3D(m);
    detail::CheckMatLinear3D(rotation);

    vector_type x, y, z;
    matrix_get_basis_vectors(m, x, y, z);

    scale_x = x.length();
    scale_y = y.length();
    scale_z = z.length();

    x /= scale_x;
    y /= scale_y;
    z /= scale_z;

    matrix_set_basis_vectors(rotation, x, y, z);
    translation = matrix_get_translation(m);
}

/* Quaternion version */
template <
    class MatT,
    typename Real,
    typename QE,
    class QA,
    class O,
    class C,
    typename VE,
    class VA
    >
void matrix_decompose_SRT(
    const MatT& m,
    Real& scale_x,
    Real& scale_y,
    Real& scale_z,
    quaternion<QE,QA,O,C>& rotation,
    vector<VE,VA>& translation)
{
    typedef MatT matrix_type;
    [[maybe_unused]] typedef typename matrix_type::value_type value_type;
    typedef matrix< value_type, fixed<3,3> > rotation_type;

    rotation_type rotation_matrix;
    matrix_decompose_SRT(
        m, scale_x, scale_y, scale_z, rotation_matrix, translation);
    quaternion_rotation_matrix(rotation, rotation_matrix);
}

/* Euler angle version */
template < class MatT, typename Real, typename E, class A >
void matrix_decompose_SRT(
    const MatT& m,
    Real& scale_x,
    Real& scale_y,
    Real& scale_z,
    Real& angle_0,
    Real& angle_1,
    Real& angle_2,
    EulerOrder order,
    vector<E,A>& translation,
    Real tolerance = epsilon<Real>::placeholder())
{
    typedef MatT matrix_type;
    [[maybe_unused]] typedef typename matrix_type::value_type value_type;
    typedef matrix< value_type, fixed<3,3> > rotation_type;

    rotation_type rotation_matrix;
    matrix_decompose_SRT(
        m, scale_x, scale_y, scale_z, rotation_matrix, translation);
    matrix_to_euler(
        rotation_matrix, angle_0, angle_1, angle_2, order, tolerance);
}

/* Axis-angle version */
template < class MatT, typename Real, typename E, class A >
void matrix_decompose_SRT(
    const MatT& m,
    Real& scale_x,
    Real& scale_y,
    Real& scale_z,
    vector<E,A>& axis,
    Real& angle,
    vector<E,A>& translation,
    Real tolerance = epsilon<Real>::placeholder())
{
    typedef MatT matrix_type;
    [[maybe_unused]] typedef typename matrix_type::value_type value_type;
    typedef matrix< value_type, fixed<3,3> > rotation_type;

    rotation_type rotation_matrix;
    matrix_decompose_SRT(
        m, scale_x, scale_y, scale_z, rotation_matrix, translation);
    matrix_to_axis_angle(rotation_matrix, axis, angle, tolerance);
}

/* 2x2 matrix version, 2-d */
template <
    class MatT,
    typename Real,
    typename ME,
    class MA,
    class B,
    class L,
    typename VE,
    class VA
    >
void matrix_decompose_SRT_2D(
    const MatT& m,
    Real& scale_x,
    Real& scale_y,
    matrix<ME,MA,B,L>& rotation,
    vector<VE,VA>& translation)
{
    typedef MatT matrix_type;
    [[maybe_unused]] typedef typename matrix_type::value_type value_type;
    [[maybe_unused]] typedef vector<value_type, fixed<2> > vector_type;

    /* Checking */
    detail::CheckMatAffine2D(m);
    detail::CheckMatLinear2D(rotation);

    vector_type x, y;
    matrix_get_basis_vectors_2D(m, x, y);

    scale_x = x.length();
    scale_y = y.length();

    x /= scale_x;
    y /= scale_y;

    matrix_set_basis_vectors_2D(rotation, x, y);
    translation = matrix_get_translation_2D(m);
}

/* Angle version, 2-d */
template < class MatT, typename Real, typename E, class A >
void matrix_decompose_SRT_2D(
    const MatT& m,
    Real& scale_x,
    Real& scale_y,
    Real& angle,
    vector<E,A>& translation)
{
    typedef MatT matrix_type;
    [[maybe_unused]] typedef typename matrix_type::value_type value_type;
    typedef matrix< value_type, fixed<2,2> > rotation_type;

    rotation_type rotation_matrix;
    matrix_decompose_SRT_2D(
        m, scale_x, scale_y, rotation_matrix, translation);
    angle = matrix_to_rotation_2D(rotation_matrix);
}

} // namespace cml

#endif
