#include <rtems.h>

#include "tmtc_pool.h"

/**
 * \brief Maximum length of the memory block.
 */
#define TMTC_MAX_LENGTH       256

/**
 * \brief Maximum number of elements that can be allocated.
 */
#define TMTC_POOL_MAX_NOE    20

/**
 * \brief A memory pool for managing blocks of 256 unsigned 8-bit integers.
 *
 * This structure keeps track of the available free blocks and provides an
 * array of pre-allocated blocks for usage.
 */
struct tmtc_pool {

    uint8_t free_blocks[TMTC_POOL_MAX_NOE];

    uint8_t blocks[TMTC_POOL_MAX_NOE][TMTC_MAX_LENGTH];

};

/**
 * \brief The one and only instance of the pool.
 */
static struct tmtc_pool the_tmtc_pool;


/**
 * \brief Identifier of the pool mutex.
 */
static rtems_id tmtc_pool_mutex_id;


rtems_status_code init_tmtc_pool() {

    rtems_status_code status = RTEMS_SUCCESSFUL;

    //Mark all blocks in the memory pool as free
    for (uint8_t i = 0; i<TMTC_POOL_MAX_NOE; i++)
    {
    	the_tmtc_pool.free_blocks[i] = 1;
    }

    status = rtems_semaphore_create(
        rtems_build_name('D','C','P','L'),
        1,
        RTEMS_BINARY_SEMAPHORE,
        0,
        &tmtc_pool_mutex_id
    );


    return status;
}


uint8_t * tmtc_pool_alloc() {

    uint8_t * ret = NULL;

    // Perform a linear search until finding the first free block. The
    // address of the block will be stored in ret and returned to the task.
    for (uint8_t i = 0; i<TMTC_POOL_MAX_NOE; i++)
        {
        	if(the_tmtc_pool.free_blocks[i]){
        		rtems_semaphore_obtain(tmtc_pool_mutex_id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
        		ret = &the_tmtc_pool.blocks[i];
        		rtems_semaphore_release(tmtc_pool_mutex_id);
        		break;
        	}
        }

    return ret;

}


void tmtc_pool_free(uint8_t * p_block) {

    uint32_t index, alignment;

    // Obtain the index of the block within the array
    index = (p_block - the_tmtc_pool.blocks[0]) / TMTC_MAX_LENGTH;
    alignment = (p_block - the_tmtc_pool.blocks[0]) % TMTC_MAX_LENGTH;

    // Check that the index is within the limits of the memory pool and that
    // the address is aligned to the beginning of a block.
    if (index < TMTC_POOL_MAX_NOE && alignment == 0) {

        // Mark the block as free
    	rtems_semaphore_obtain(tmtc_pool_mutex_id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
        the_tmtc_pool.free_blocks[index] = 1;
        rtems_semaphore_release(tmtc_pool_mutex_id);

    }

}
