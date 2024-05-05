#include "BasePipeline.h"

BasePipeline::BasePipeline(BasePipeline* super) {
  this->descriptors = super->descriptors;
}
