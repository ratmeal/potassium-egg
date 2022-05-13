# Download the latest Limine binary release.
git clone https://github.com/limine-bootloader/limine.git --branch=v3.0-branch-binary --depth=1
 
# Build limine-deploy.
make -C limine
mkdir -p iso_root
cp -v photon.elf limine.cfg limine/limine.sys \
      limine/limine-cd.bin limine/limine-cd-efi.bin iso_root/
 
# Create the bootable ISO.
xorriso -as mkisofs -b limine-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table \
        --efi-boot limine-cd-efi.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        iso_root -o image.iso
 
# Install Limine stage 1 and 2 for legacy BIOS boot.
./limine/limine-deploy image.iso