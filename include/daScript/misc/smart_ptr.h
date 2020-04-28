#pragma once

namespace das {

    template <typename T>
    struct smart_ptr_raw {
        smart_ptr_raw () {}
        smart_ptr_raw ( T * p ) : ptr(p) {}
        __forceinline T * get() const {
            return ptr;
        }
        __forceinline T * operator -> () const {
            return ptr;
        }
        __forceinline operator smart_ptr_raw<void> & () const {
            return *((smart_ptr_raw<void> *)this);
        }
        T * ptr;
    };

    template <typename T>
    struct smart_ptr_policy {
        __forceinline static void addRef ( T * p ) { p->addRef(); }
        __forceinline static bool delRef ( T * p ) { return p->delRef(); }
        __forceinline static unsigned int use_count ( T * p ) { return p->use_count(); }
        __forceinline static T & get_value ( T * p ) { return *p; }
    };

    template <>
    struct smart_ptr_policy<void> {
        __forceinline static void addRef ( void * ) { }
        __forceinline static bool delRef ( void * ) { return false; }
        __forceinline static unsigned int use_count ( void * ) { return 0; }
        __forceinline static void * get_value ( void * p ) { return p; }
    };

    template<typename T, typename TP = smart_ptr_policy<T>>
    class smart_ptr {
    public:
        using element_type = T;
        using element_type_ptr = T *;
        __forceinline smart_ptr ( ) {
            ptr = nullptr;
        }
        __forceinline smart_ptr ( const smart_ptr & p ) {
            init(p.ptr);
        }
        template <typename Y>
        __forceinline smart_ptr ( const smart_ptr<Y> & p ) {
            init(p.get());
        }
        __forceinline operator smart_ptr_raw<void> & () const {
            return *((smart_ptr_raw<void> *)this);
        }
        __forceinline operator smart_ptr_raw<T> & () const {
            return *((smart_ptr_raw<T> *)this);
        }
        __forceinline smart_ptr ( smart_ptr && p ) {
            ptr = p.ptr;
            p.ptr = nullptr;
        }
        __forceinline smart_ptr ( T * p ) {
            init(p);
        }
        __forceinline ~smart_ptr() {
            reset();
        }
        __forceinline smart_ptr & operator = ( const smart_ptr & p ) {
            return set(p.ptr);
        }
        template <typename Y>
        __forceinline smart_ptr & operator = ( const smart_ptr<Y> & p ) {
            return set(p.get());
        }
        __forceinline smart_ptr & operator = ( smart_ptr && p ) {
			reset();
            ptr = p.ptr;
            p.ptr = nullptr;
            return *this;
        }
        __forceinline smart_ptr & operator = ( T * p ) {
            return set(p);
        }
        __forceinline void reset() {
            T * t = ptr;
            ptr = nullptr;
            if ( t ) TP::delRef(t);
        }
        __forceinline void swap ( smart_ptr & p ) {
            T * t = ptr;
            ptr = p.ptr;
            p.ptr = t;
        }
        __forceinline void reset ( T * p ) {
            set(p);
        }
        __forceinline decltype(TP::get_value(element_type_ptr())) operator * () const {
            return TP::get_value(ptr);
        }
        __forceinline T * operator -> () const {
            return ptr;
        }
        __forceinline T * get() const {
            return ptr;
        }
        __forceinline operator bool() const {
            return ptr != nullptr;
        }
        __forceinline bool operator == ( T * p ) const {
            return ptr == p;
        }
        template <typename Y>
        __forceinline bool operator == ( const smart_ptr<Y> & p ) const {
            return ptr == p.get();
        }
        __forceinline bool operator != ( T * p ) const {
            return ptr != p;
        }
        template <typename Y>
        __forceinline bool operator != ( const smart_ptr<Y> & p ) const {
            return ptr != p.get();
        }
        __forceinline bool operator >= ( T * p ) const {
            return ptr >= p;
        }
        template <typename Y>
        __forceinline bool operator >= ( const smart_ptr<Y> & p ) const {
            return ptr >= p.get();
        }
        __forceinline bool operator <= ( T * p ) const {
            return ptr <= p;
        }
        template <typename Y>
        __forceinline bool operator <= ( const smart_ptr<Y> & p ) const {
            return ptr <= p.get();
        }
        __forceinline bool operator > ( T * p ) const {
            return ptr > p;
        }
        template <typename Y>
        __forceinline bool operator > ( const smart_ptr<Y> & p ) const {
            return ptr > p.get();
        }
        __forceinline bool operator < ( T * p ) const {
            return ptr < p;
        }
        template <typename Y>
        __forceinline bool operator < ( const smart_ptr<Y> & p ) const {
            return ptr < p.get();
        }
    protected:
        __forceinline smart_ptr & set ( T * p )  {
            T * t = ptr;
            ptr = p;
            addRef();
            if ( t ) TP::delRef(t);
            return *this;
        }
        __forceinline void init ( T * p = nullptr )  {
            ptr = p;
            addRef();
        }
        __forceinline void addRef() {
            if ( ptr ) TP::addRef(ptr);
        }
        __forceinline void delRef() {
            if ( ptr && TP::delRef(ptr) ) ptr = nullptr;
        }
    protected:
        T * ptr;
    };

    template <class T, class U>
    __forceinline bool operator == (T * l, const smart_ptr<U> & r) {
        return l == r.get();
    }
    template <class T, class U>
    __forceinline bool operator != (T * l, const smart_ptr<U> & r) {
        return l != r.get();
    }
    template <class T, class U>
    __forceinline bool operator >= (T * l, const smart_ptr<U> & r) {
        return l >= r.get();
    }
    template <class T, class U>
    __forceinline bool operator <= (T * l, const smart_ptr<U> & r) {
        return l <= r.get();
    }
    template <class T, class U>
    __forceinline bool operator > (T * l, const smart_ptr<U> & r) {
        return l > r.get();
    }
    template <class T, class U>
    __forceinline bool operator < (T * l, const smart_ptr<U> & r) {
        return l < r.get();
    }

    template< class T, class... Args >
    __forceinline smart_ptr<T> make_smart ( Args&&... args ) {
        T * p = new T(args...);
        return smart_ptr<T>(p);
    }

    template< class T, class U >
    __forceinline smart_ptr<T> static_pointer_cast(const smart_ptr<U> & r) {
        return static_cast<T *>(r.get());
    }

    template< class T, class U >
    __forceinline smart_ptr<T> dynamic_pointer_cast(const smart_ptr<U> & r) {
        return dynamic_cast<T *>(r.get());
    }

    template< class T, class U >
    __forceinline smart_ptr<T> const_pointer_cast(const smart_ptr<U> & r) {
        return const_cast<T *>(r.get());
    }

    template< class T, class U >
    __forceinline smart_ptr<T> reinterpret_pointer_cast(const smart_ptr<U> & r) {
        return reinterpret_cast<T *>(r.get());
    }

    class ptr_ref_count {
    public:
        __forceinline ptr_ref_count () {}
        __forceinline ptr_ref_count ( const ptr_ref_count &  ) {}
        __forceinline ptr_ref_count ( const ptr_ref_count && ) {}
        __forceinline ptr_ref_count & operator = ( const ptr_ref_count & ) { return *this;}
        __forceinline ptr_ref_count & operator = ( ptr_ref_count && ) { return *this; }
        virtual ~ptr_ref_count() {
            DAS_ASSERTF(ref_count == 0, "can only delete when ref_count==0");
        }
        __forceinline void addRef() {
            ref_count ++;
            DAS_ASSERTF(ref_count, "ref_count overflow");
        }
        __forceinline bool delRef() {
            DAS_ASSERTF(ref_count, "deleting reference on the object with ref_count==0");
            if ( --ref_count==0 ) {
                delete this;
                return true;
            } else {
                return false;
            }
        }
        __forceinline unsigned int use_count() const {
            return ref_count;
        }
    private:
        unsigned int ref_count = 0;
    };
}

namespace std {
    template <typename TT>
    struct hash<das::smart_ptr<TT>> {
        std::size_t operator() ( const das::smart_ptr<TT> & k ) const {
            return hash<void *>()(k.get());
        }
    };
}