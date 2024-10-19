#include "hv.h"

#include <ntddk.h>
#include <ia32.hpp>
#include "log.h"

// simple hypercall wrappers
static uint64_t ping() {
  hv::hypercall_input input;
  input.code = hv::hypercall_ping;
  input.key  = hv::hypercall_key;
  return hv::vmx_vmcall(input);
}

void driver_unload(PDRIVER_OBJECT) {
  hv::stop();

  LOG("[hv] Devirtualized the system.\n");
  LOG("[hv] Driver unloaded.\n");
}

NTSTATUS driver_entry(PDRIVER_OBJECT const driver, PUNICODE_STRING) {
    LOG("[hv] Driver loaded.\n");

  if (driver)
    driver->DriverUnload = driver_unload;

  if (!hv::start()) {
      LOG("[hv] Failed to virtualize system.\n");
    return STATUS_HV_OPERATION_FAILED;
  }

  if (ping() == hv::hypervisor_signature)
      LOG("[client] Hypervisor signature matches.\n");
  else
      LOG("[client] Failed to ping hypervisor!\n");

  return STATUS_SUCCESS;
}

