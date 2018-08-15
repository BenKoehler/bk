/*
 * MIT License
 *
 * Copyright (c) 2018 Benjamin Köhler
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#ifndef BKGL_ABSTRACTBINDABLE_H
#define BKGL_ABSTRACTBINDABLE_H

#include <bkGL/AbstractObject.h>

namespace bk::details
{
    class BKGL_EXPORT AbstractBindable : public AbstractObject
    {
        //====================================================================================================
        //===== DEFINITIONS
        //====================================================================================================
        using self_type = AbstractBindable;
        using base_type = AbstractObject;

        //====================================================================================================
        //===== MEMBERS
        //====================================================================================================
      protected:
        bool _is_bound;
        GLuint _id;

        //====================================================================================================
        //===== CONSTRUCTORS & DESTRUCTOR
        //====================================================================================================
    public:
        /// @{ -------------------------------------------------- CTOR
        #ifndef BK_LIB_QT_AVAILABLE
        AbstractBindable();
        #else
        AbstractBindable(qt_gl_functions* gl);
        #endif
        AbstractBindable(const self_type& other) = delete;
        AbstractBindable(self_type&& other);
        /// @}

        /// @{ -------------------------------------------------- DTOR
        virtual ~AbstractBindable();
        /// @}

        //====================================================================================================
        //===== GETTER
        //====================================================================================================
        /// @{ -------------------------------------------------- IS BOUND
        [[nodiscard]] bool is_bound() const;
        /// @}

        /// @{ -------------------------------------------------- GET ID
        [[nodiscard]] GLuint id() const;
        /// @}

        /// @{ -------------------------------------------------- IS INITIALIZED
        [[nodiscard]] virtual bool is_initialized() const override;
        /// @}

        //====================================================================================================
        //===== SETTER
        //====================================================================================================
        /// @{ -------------------------------------------------- OPERATOR =
        [[maybe_unused]] auto operator=(const self_type& other) -> self_type& = delete;
        [[maybe_unused]] auto operator=(self_type&& other) -> self_type&;
        /// @}

        //====================================================================================================
        //===== FUNCTIONS
        //====================================================================================================
        /// @{ -------------------------------------------------- CLEAR
        void clear();
    protected:
        virtual void clear_impl() = 0;
    public:
        /// @}

        /// @{ -------------------------------------------------- BIND
        void bind();
    protected:
        virtual void bind_impl() = 0;
    public:
        /// @}

        /// @{ -------------------------------------------------- RELEASE
        void release();
      protected:
        virtual void release_impl() = 0;
      public:
        /// @}
    }; // class AbstractBindable
} // namespace bk::details

#endif //BKGL_ABSTRACTBINDABLE_H


