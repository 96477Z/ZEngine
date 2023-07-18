#ifndef Z_MEMORY_POOL_INTERNAL_Z_LIST_MEMORY_POOL_BASE_H_
#define Z_MEMORY_POOL_INTERNAL_Z_LIST_MEMORY_POOL_BASE_H_

#include "internal/drive.h"

#include "z_memory_pool_base.h"
#include "z_heap_memory_pool.h"

namespace zengine {
namespace memory_pool {

/*
    The base type of the memory pool that use the structure of list pool.
    Waring: The program will crash if a nullptr is send in.
    Template Parameters:
    - MemoryBlockType: The memory block type of the memory pool.
    - kMemoryBlockHeadOffset: The memory might not start at the start of the 
        memory block, it's the offset to the real memory.
    - kIsThreadSafe: Thread safe or not.
*/
template<typename MemoryBlockType, PointerType kMemoryBlockHeadOffset, Bool kIsThreadSafe>
class ZListMemoryPoolBase :public ZMemoryPoolBase<kIsThreadSafe> {
private:
    //The multipul factor that container auto extends based on the origin size.
    static constexpr Float32 kAutoExtendMulFactor = 0.2F;
    //The min number the container auto extends at least.
    static constexpr IndexType kAutoExtendMinNum = 1;
    //The max size applied one time when extending.
    static constexpr MemoryType kApplyHeapMemoryMaxSizePurTime = 4 * kMB;
    //The unit size of the applied memory.
    static constexpr MemoryType kApplyHeapMemoryUnitSize = 4 * kHeapMemoryUnitSize;

protected:
    using SuperType = ZMemoryPoolBase<kIsThreadSafe>;
    using MutexType = ZMemoryPoolThreadSafeBase<kIsThreadSafe>;
    
    struct Node{
        Node* next_node_ptr;
        MemoryBlockType memory_block;
    };

#pragma warning(disable : 26495)
    FORCEINLINE ZListMemoryPoolBase() : SuperType() {}
#pragma warning(default : 26495)
    Void Initialize(const MemoryPoolType memory_pool_type, const MemoryType memory_block_size,
                    const MemoryType memory_block_memory_size, const IndexType capacity) noexcept;
    
    FORCEINLINE static constexpr MemoryType node_head_offset() { return kNodeHeadOffset; }
    NODISCARD FORCEINLINE const MemoryType capacity() const { return capacity_; }
    NODISCARD FORCEINLINE const MemoryType memory_block_size() const { return memory_block_size_; }
    NODISCARD FORCEINLINE const MemoryType memory_block_memory_size() const { return memory_block_memory_size_; }

    /*
        It returns the memory's pointer(not the memory block).It's inlined
        cause it's only used a few times.
    */
    NODISCARD FORCEINLINE Void* const ApplyMemory();

    /*
        Release the memory to this memory pool. It's inlinedcause it's only 
        used a few times.
    */
    FORCEINLINE Void ReleaseMemory(const Void* const memory_ptr);

private:
    //The total offset of the memory block.
    static constexpr PointerType kNodeHeadOffset = sizeof(Node*) + kMemoryBlockHeadOffset;

    ZListMemoryPoolBase(const ZListMemoryPoolBase&) = delete;
    ZListMemoryPoolBase(ZListMemoryPoolBase&&) = delete;

    ZListMemoryPoolBase& operator=(const ZListMemoryPoolBase&) = delete;
    ZListMemoryPoolBase& operator=(ZListMemoryPoolBase&&) = delete;

    /*
        Called when the memory pool runs out. It aoto extends the memory pool.
    */
    FORCEINLINE Void AutoExtendCapcity();

    /*
        The function that extends the memory pool. It must be rewrited in the
        sub class. capacity_ and head_node_ptr_ will be changed in this function.
    */
    Void ExtendCapacity(const IndexType memory_block_added_num) noexcept;

    //The size of the memory block(include the usable memory size)
    MemoryType memory_block_size_;
    //The size of the useable memory.
    MemoryType memory_block_memory_size_;

    IndexType capacity_;
    Node* head_node_ptr_;
};

template<typename MemoryBlockType, PointerType kMemoryBlockHeadOffset, Bool kIsThreadSafe>
Void ZListMemoryPoolBase<MemoryBlockType, kMemoryBlockHeadOffset, kIsThreadSafe>::Initialize(
        const MemoryPoolType memory_pool_type, const MemoryType memory_block_size,
        const MemoryType memory_block_memory_size, const IndexType capacity) noexcept {
    SuperType::Initialize(memory_pool_type);
    memory_block_size_ = memory_block_size;
    memory_block_memory_size_ = memory_block_memory_size;
    head_node_ptr_ = nullptr;
    capacity_ = capacity;
    ExtendCapacity(capacity);
}


template<typename MemoryBlockType, PointerType kMemoryBlockHeadOffset, Bool kIsThreadSafe>
NODISCARD FORCEINLINE Void* const ZListMemoryPoolBase<MemoryBlockType, kMemoryBlockHeadOffset, kIsThreadSafe>::ApplyMemory() {
    MutexType::lock();
    if (head_node_ptr_ == nullptr) {
        AutoExtendCapcity();
    }
    Void* const memory_ptr = reinterpret_cast<Void*>(reinterpret_cast<PointerType>(head_node_ptr_) + kNodeHeadOffset);
    head_node_ptr_ = head_node_ptr_->next_node_ptr;
    MutexType::unlock();
    return memory_ptr;
}

template<typename MemoryBlockType, PointerType kMemoryBlockHeadOffset, Bool kIsThreadSafe>
FORCEINLINE Void ZListMemoryPoolBase<MemoryBlockType, kMemoryBlockHeadOffset, kIsThreadSafe>::ReleaseMemory(
        const Void* const memory_ptr) {
    MutexType::lock();
    Node* const node_ptr = reinterpret_cast<Node*>(reinterpret_cast<PointerType>(memory_ptr) - kNodeHeadOffset);
    node_ptr->next_node_ptr = head_node_ptr_;
    head_node_ptr_ = node_ptr;
    MutexType::unlock();
}

template<typename MemoryBlockType, PointerType kMemoryBlockHeadOffset, Bool kIsThreadSafe>
FORCEINLINE Void ZListMemoryPoolBase<MemoryBlockType, kMemoryBlockHeadOffset, kIsThreadSafe>::AutoExtendCapcity() {
    IndexType extend_num = static_cast<IndexType>(capacity_ * kAutoExtendMulFactor);
    if (extend_num < kAutoExtendMinNum) {
        extend_num = kAutoExtendMinNum;
    }
    ExtendCapacity(extend_num);
}

template<typename MemoryBlockType, PointerType kMemoryBlockHeadOffset, Bool kIsThreadSafe>
Void ZListMemoryPoolBase<MemoryBlockType, kMemoryBlockHeadOffset, kIsThreadSafe>::ExtendCapacity(
        const IndexType memory_block_added_num) noexcept {
    if (memory_block_added_num == 0) {
        return;
    }
    //Calculates the size that needs to apply. Rounds up to the unit size's multiple.
    MemoryType apply_heap_memory_size = memory_block_added_num * memory_block_size_;
    if (apply_heap_memory_size >= kApplyHeapMemoryMaxSizePurTime) {
        apply_heap_memory_size = kApplyHeapMemoryMaxSizePurTime;
    }
    else if ((apply_heap_memory_size & (kApplyHeapMemoryUnitSize - 1)) != 0) {
        apply_heap_memory_size = (apply_heap_memory_size & (~(kApplyHeapMemoryUnitSize - 1)))
            + kApplyHeapMemoryUnitSize;
    }
    //Apply heap memory.
    Void* const apply_memory_ptr = ZHeapMemoryPool<kIsThreadSafe>::ApplyMemory(apply_heap_memory_size);
    PointerType temp_memory_ptr = reinterpret_cast<PointerType>(apply_memory_ptr);
    SuperType* const this_memory_pool_ptr = static_cast<SuperType*>(this);
    //Recaculate the real memory block num added. 
    IndexType apply_memory_block_num = apply_heap_memory_size / memory_block_size_; 
    capacity_ += apply_memory_block_num;
    //Initialize the memory block.
    for (IndexType count = 1; count < apply_memory_block_num; count++) {
        //Initialize the memory block.
        reinterpret_cast<Node*>(temp_memory_ptr)->memory_block.Initialize(reinterpret_cast<Void*>(this));
        //Links the blocks into a list.
        reinterpret_cast<Node*>(temp_memory_ptr)->next_node_ptr =
            reinterpret_cast<Node*>(temp_memory_ptr + memory_block_size_);
        //Next memory block start pointer.
        temp_memory_ptr += memory_block_size_;
    }
    //Initialize the last memory block.
    reinterpret_cast<Node*>(temp_memory_ptr)->memory_block.Initialize(reinterpret_cast<Void*>(this));
    //Puts the blocks into the memory pool.
    reinterpret_cast<Node*>(temp_memory_ptr)->next_node_ptr =
        const_cast<Node*>(head_node_ptr_);
    head_node_ptr_ = reinterpret_cast<Node*>(apply_memory_ptr);
}

}//memory_pool
}//zengine

#endif // !Z_MEMORY_POOL_INTERNAL_Z_LIST_MEMORY_POOL_BASE_H_
