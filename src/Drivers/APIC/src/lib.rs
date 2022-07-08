#![no_std]
#![no_main]
use core::panic::PanicInfo;
use x2apic::lapic::{LocalApic, LocalApicBuilder, xapic_base, TimerDivide, TimerMode};
#[no_mangle]
pub extern "C" fn lapicinit(hhdm: u64)
{
    // x2apic is not for consumers
    // it'll fall back to xapic anyway so we're fine
    let apic_physical_address = unsafe {xapic_base()};
    let apic_virtual_address = apic_physical_address + hhdm;
    let mut lapic = LocalApicBuilder::new()
    .timer_vector(32)
    .error_vector(33)
    .spurious_vector(34)
    .set_xapic_base(apic_virtual_address) // set this to virtual address with the higher half
    .build()
    .unwrap_or_else(|err| panic!("{}", err));
    unsafe
    {
        lapic.enable();
    }
    // test out the timer by counting down to 0 from 10 seconds
}

#[panic_handler]
fn panic(_panic: &PanicInfo<'_>) -> ! {
    loop {}
}