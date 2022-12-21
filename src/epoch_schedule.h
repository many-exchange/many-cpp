#pragma once

#include <vector>

#include "publickey.h"

namespace solana {

/**
 * Epoch schedule
 * (see https://docs.solana.com/terminology#epoch)
 * Can be retrieved with the {@link connection.getEpochSchedule} method
 */
struct EpochSchedule {
  /** The maximum number of slots in each epoch */
  uint64_t slots_per_epoch;
  /** The number of slots before beginning of an epoch to calculate a leader schedule for that epoch */
  uint64_t leader_schedule_slot_offset;
  /** Indicates whether epochs start short and grow */
  bool warmup;
  /** The first epoch with `slotsPerEpoch` slots */
  bool first_normal_epoch;
  /** The first slot of `firstNormalEpoch` */
  uint64_t first_normal_slot;

  //TODO
  /*
  getEpoch(slot: number): number {
    return this.getEpochAndSlotIndex(slot)[0];
  }

  getEpochAndSlotIndex(slot: number): [number, number] {
    if (slot < this.firstNormalSlot) {
      const epoch =
        trailingZeros(nextPowerOfTwo(slot + MINIMUM_SLOT_PER_EPOCH + 1)) -
        trailingZeros(MINIMUM_SLOT_PER_EPOCH) -
        1;

      const epochLen = this.getSlotsInEpoch(epoch);
      const slotIndex = slot - (epochLen - MINIMUM_SLOT_PER_EPOCH);
      return [epoch, slotIndex];
    } else {
      const normalSlotIndex = slot - this.firstNormalSlot;
      const normalEpochIndex = Math.floor(normalSlotIndex / this.slotsPerEpoch);
      const epoch = this.firstNormalEpoch + normalEpochIndex;
      const slotIndex = normalSlotIndex % this.slotsPerEpoch;
      return [epoch, slotIndex];
    }
  }

  getFirstSlotInEpoch(epoch: number): number {
    if (epoch <= this.firstNormalEpoch) {
      return (Math.pow(2, epoch) - 1) * MINIMUM_SLOT_PER_EPOCH;
    } else {
      return (
        (epoch - this.firstNormalEpoch) * this.slotsPerEpoch +
        this.firstNormalSlot
      );
    }
  }

  getLastSlotInEpoch(epoch: number): number {
    return this.getFirstSlotInEpoch(epoch) + this.getSlotsInEpoch(epoch) - 1;
  }

  getSlotsInEpoch(epoch: number) {
    if (epoch < this.firstNormalEpoch) {
      return Math.pow(2, epoch + trailingZeros(MINIMUM_SLOT_PER_EPOCH));
    } else {
      return this.slotsPerEpoch;
    }
  }
  */

};

}
