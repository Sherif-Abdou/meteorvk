#include "DescriptorManager.h"

DescriptorManager::DescriptorManager(VulkanContext* context): context(context) {};


void DescriptorManager::addDescriptorSet(const std::string& name, DescriptorSet* set) {
  this->descriptors[name] = set;
}

DescriptorSet* DescriptorManager::getDescriptorSet(const std::string& name) {
  if (descriptors.find(name) == descriptors.end()) {
    return nullptr;
  }
  return descriptors[name];
}


bool DescriptorManager::containsMandatoryDescriptors(const std::vector<std::string>& name_list) {
  for (const auto& name: name_list) {
    if (getDescriptorSet(name) == nullptr) {
      return false;
    }
  }
  return true;
}
