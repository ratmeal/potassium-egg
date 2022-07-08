#![no_std]
#![no_main]
use core::panic::PanicInfo;
use x2apic::lapic::{LocalApic, LocalApicBuilder, xapic_base};
#[no_mangle]
pub extern "C" fn test(hhdm: u64)
{
    let apic_physical_address: u64 = unsafe { xapic_base() };
    let apic_virtual_address: u64 = apic_physical_address + hhdm;
    let mut lapic = LocalApicBuilder::new()
        .timer_vector(80)
        .error_vector(81)
        .spurious_vector(82)
        .set_xapic_base(apic_virtual_address)
        .build()
        .unwrap_or_else(|err| panic!("{}", err));

    unsafe {
        lapic.enable();
        lapic.disable_timer();
    }
}

#[panic_handler]
fn panic(_panic: &PanicInfo<'_>) -> ! {
    loop {}
}