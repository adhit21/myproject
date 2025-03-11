#include <stdio.h>
#include <assert.h>
#include <libusb-1.0/libusb.h>

int main(int argc, char **argv) {
    libusb_context *context;
    libusb_device_handle *handle;
    libusb_device *device;
    struct libusb_device_descriptor desc;
    unsigned char buffer[256];
    int fd;

    assert((argc > 1) && (sscanf(argv[1], "%d", &fd) == 1));

    libusb_set_option(NULL, LIBUSB_OPTION_NO_DEVICE_DISCOVERY);
    assert(!libusb_init(&context));
    assert(!libusb_wrap_sys_device(context, (intptr_t) fd, &handle));

    device = libusb_get_device(handle);
    assert(!libusb_get_device_descriptor(device, &desc));

    // Informasi dasar
    printf("Vendor ID: %04x\n", desc.idVendor);
    printf("Product ID: %04x\n", desc.idProduct);

    if (libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, buffer, sizeof(buffer)) >= 0) {
        printf("Manufacturer: %s\n", buffer);
    }

    if (libusb_get_string_descriptor_ascii(handle, desc.iProduct, buffer, sizeof(buffer)) >= 0) {
        printf("Product: %s\n", buffer);
    }

    if (libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, buffer, sizeof(buffer)) >= 0) {
        printf("Serial No: %s\n", buffer);
    }

    // Tambahan Informasi
    printf("\n--- Additional Device Info ---\n");
    printf("USB Class: %02x\n", desc.bDeviceClass);
    printf("USB Subclass: %02x\n", desc.bDeviceSubClass);
    printf("USB Protocol: %02x\n", desc.bDeviceProtocol);
    printf("Number of Configurations: %d\n", desc.bNumConfigurations);
    printf("Max Packet Size (Endpoint 0): %d bytes\n", desc.bMaxPacketSize0);

    // Cek kecepatan perangkat (hanya bisa kalau libusb >= 1.0.9)
    int speed = libusb_get_device_speed(device);
    const char *speed_str = "Unknown";
    switch (speed) {
        case LIBUSB_SPEED_LOW: speed_str = "Low (1.5 Mbps)"; break;
        case LIBUSB_SPEED_FULL: speed_str = "Full (12 Mbps)"; break;
        case LIBUSB_SPEED_HIGH: speed_str = "High (480 Mbps)"; break;
        case LIBUSB_SPEED_SUPER: speed_str = "Super (5 Gbps)"; break;
        case LIBUSB_SPEED_SUPER_PLUS: speed_str = "Super+ (10 Gbps)"; break;
    }
    printf("Device Speed: %s\n", speed_str);

    // Ambil daya yang dipakai dari konfigurasi pertama
    struct libusb_config_descriptor *config;
    if (libusb_get_config_descriptor(device, 0, &config) == 0) {
        printf("Max Power: %d mA\n", config->MaxPower * 2);
        libusb_free_config_descriptor(config);
    } else {
        printf("Max Power: Unknown\n");
    }

    libusb_exit(context);
    return 0;
}
