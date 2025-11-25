/*
 * Wideband SDR - USB 2.0 High-Speed Device Stack
 * dsPIC33AK256MC505 based SDR with USB 2.0 High-Speed interface
 * 
 * Created: November 2025
 * Version: 1.0.0
 * License: MIT
 * 
 * Features:
 * - USB 2.0 High-Speed (480 Mbps) bulk transfers
 * - Vendor-specific device class
 * - 512-byte endpoint packets
 * - Flow control and back-pressure handling
 * - Hot-plug detection and recovery
 * - Automatic enumeration
 */

#ifndef USB_DEVICE_H
#define USB_DEVICE_H

#include <stdint.h>
#include <stdbool.h>

// USB Configuration
#define USB_VENDOR_ID          0x04D8        // Microchip VID (example)
#define USB_PRODUCT_ID         0x000A        // Wideband SDR PID
#define USB_MAX_POWER          250           // 500 mA (250 * 2mA units)
#define USB_CONFIG_DESCRIPTOR_TYPE  0x02
#define USB_INTERFACE_DESCRIPTOR_TYPE 0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE  0x05

// USB Endpoint Definitions
#define USB_ENDPOINT_CONTROL   0             // Control endpoint (EP0)
#define USB_ENDPOINT_BULK_IN   1             // Bulk IN endpoint (EP1)
#define USB_ENDPOINT_BULK_OUT  2             // Bulk OUT endpoint (EP2)
#define USB_ENDPOINT_INTERRUPT_IN 3          // Interrupt IN endpoint (EP3)

// USB Packet and Buffer Sizes
#define USB_MAX_PACKET_SIZE    512           // High-Speed bulk max packet size
#define USB_CONTROL_MAX_SIZE   64            // Control transfer max packet size
#define USB_BUFFER_SIZE        2048          // USB transfer buffer size
#define USB_NUM_BUFFERS        4             // Number of transfer buffers

// USB Request Types
#define USB_TYPE_STANDARD      (0 << 5)
#define USB_TYPE_CLASS         (1 << 5)
#define USB_TYPE_VENDOR        (2 << 5)

#define USB_RECIPIENT_DEVICE   (0 << 0)
#define USB_RECIPIENT_INTERFACE (1 << 0)
#define USB_RECIPIENT_ENDPOINT (2 << 0)
#define USB_RECIPIENT_OTHER    (3 << 0)

// USB Standard Requests
#define USB_REQ_GET_STATUS         0x00
#define USB_REQ_CLEAR_FEATURE      0x01
#define USB_REQ_SET_FEATURE        0x03
#define USB_REQ_SET_ADDRESS        0x05
#define USB_REQ_GET_DESCRIPTOR     0x06
#define USB_REQ_SET_DESCRIPTOR     0x07
#define USB_REQ_GET_CONFIGURATION  0x08
#define USB_REQ_SET_CONFIGURATION  0x09

// USB Descriptor Types
#define USB_DESC_DEVICE            0x01
#define USB_DESC_CONFIGURATION     0x02
#define USB_DESC_STRING            0x03
#define USB_DESC_INTERFACE         0x04
#define USB_DESC_ENDPOINT          0x05
#define USB_DESC_DEVICE_QUALIFIER  0x06
#define USB_DESC_OTHER_SPEED       0x07
#define USB_DESC_INTERFACE_POWER   0x08

// USB Device States
typedef enum {
    USB_STATE_DETACHED = 0,
    USB_STATE_ATTACHED,
    USB_STATE_POWERED,
    USB_STATE_DEFAULT,
    USB_STATE_ADDRESS,
    USB_STATE_CONFIGURED
} usb_device_state_t;

// USB Transfer Types
typedef enum {
    USB_TRANSFER_CONTROL = 0,
    USB_TRANSFER_ISOCHRONOUS,
    USB_TRANSFER_BULK,
    USB_TRANSFER_INTERRUPT
} usb_transfer_type_t;

// USB Transfer Status
typedef enum {
    USB_TRANSFER_PENDING = 0,
    USB_TRANSFER_IN_PROGRESS,
    USB_TRANSFER_COMPLETED,
    USB_TRANSFER_ERROR,
    USB_TRANSFER_STALLED
} usb_transfer_status_t;

// USB Setup Request Structure
typedef struct {
    uint8_t  bmRequestType;    // Request type
    uint8_t  bRequest;         // Request
    uint16_t wValue;           // Value
    uint16_t wIndex;           // Index
    uint16_t wLength;          // Length
} usb_setup_t;

// USB Endpoint Structure
typedef struct {
    uint8_t  endpoint_address; // Endpoint address
    uint8_t  attributes;       // Endpoint attributes
    uint16_t max_packet_size;  // Maximum packet size
    uint8_t  interval;         // Polling interval
    void    *buffer;           // Current buffer
    uint16_t buffer_size;      // Buffer size
    uint16_t data_count;       // Data transferred
    usb_transfer_status_t status; // Transfer status
} usb_endpoint_t;

// USB Device Structure
typedef struct {
    usb_device_state_t state;              // Device state
    uint8_t           address;             // Device address
    uint8_t           configuration;       // Current configuration
    uint8_t           alternate_setting;   // Current alternate setting
    uint16_t          vendor_id;           // Vendor ID
    uint16_t          product_id;          // Product ID
    uint16_t          version_id;          // Device version
    char             manufacturer[32];     // Manufacturer string
    char             product[64];          // Product string
    char             serial_number[32];    // Serial number string
    bool             self_powered;         // Self-powered indicator
    bool             remote_wakeup;        // Remote wakeup capability
    usb_endpoint_t    endpoints[4];        // Endpoint structures
    void             *user_data;           // User-specific data
} usb_device_t;

// USB Transfer Buffer Structure
typedef struct {
    uint8_t  *buffer;           // Buffer pointer
    uint16_t  size;             // Buffer size
    uint16_t  length;           // Data length
    uint16_t  transferred;      // Bytes transferred
    uint8_t   endpoint;         // Endpoint number
    usb_transfer_type_t type;   // Transfer type
    usb_transfer_status_t status; // Transfer status
    void     *callback;         // Completion callback
    void     *user_data;        // Callback user data
} usb_transfer_t;

// USB Callback Function Types
typedef void (*usb_device_callback_t)(usb_device_t *device, void *user_data);
typedef void (*usb_transfer_callback_t)(usb_transfer_t *transfer, void *user_data);

// Function Prototypes

// Device Initialization and Management
/**
 * @brief Initialize USB device
 * @param device Pointer to USB device structure
 * @param user_data User-specific data
 * @return bool Success status
 */
bool usb_init_device(usb_device_t *device, void *user_data);

/**
 * @brief Check if USB device is connected
 * @return bool Connection status
 */
bool usb_is_connected(void);

/**
 * @brief Get current device state
 * @return usb_device_state_t Current state
 */
usb_device_state_t usb_get_state(void);

/**
 * @brief Get USB device descriptor
 * @param device Pointer to USB device
 * @param descriptor Pointer to descriptor buffer
 * @param length Pointer to descriptor length
 * @return bool Success status
 */
bool usb_get_device_descriptor(usb_device_t *device, uint8_t *descriptor, uint16_t *length);

/**
 * @brief Get configuration descriptor
 * @param device Pointer to USB device
 * @param config_index Configuration index
 * @param descriptor Pointer to descriptor buffer
 * @param length Pointer to descriptor length
 * @return bool Success status
 */
bool usb_get_configuration_descriptor(usb_device_t *device, uint8_t config_index,
                                    uint8_t *descriptor, uint16_t *length);

// Endpoint Management
/**
 * @brief Configure endpoint
 * @param device Pointer to USB device
 * @param endpoint Endpoint number
 * @param endpoint_type Transfer type
 * @param max_packet_size Maximum packet size
 * @return bool Success status
 */
bool usb_configure_endpoint(usb_device_t *device, uint8_t endpoint,
                          usb_transfer_type_t endpoint_type, uint16_t max_packet_size);

/**
 * @brief Stall endpoint
 * @param device Pointer to USB device
 * @param endpoint Endpoint number
 */
void usb_stall_endpoint(usb_device_t *device, uint8_t endpoint);

/**
 * @brief Clear endpoint stall
 * @param device Pointer to USB device
 * @param endpoint Endpoint number
 */
void usb_clear_stall(usb_device_t *device, uint8_t endpoint);

/**
 * @brief Check if endpoint is stalled
 * @param device Pointer to USB device
 * @param endpoint Endpoint number
 * @return bool Stalled status
 */
bool usb_is_endpoint_stalled(usb_device_t *device, uint8_t endpoint);

// Transfer Functions
/**
 * @brief Send data on bulk endpoint
 * @param device Pointer to USB device
 * @param endpoint Endpoint number
 * @param data Pointer to data buffer
 * @param length Data length
 * @param callback Completion callback
 * @param user_data Callback user data
 * @return bool Success status
 */
bool usb_bulk_send(usb_device_t *device, uint8_t endpoint, const uint8_t *data,
                  uint16_t length, usb_transfer_callback_t callback, void *user_data);

/**
 * @brief Receive data on bulk endpoint
 * @param device Pointer to USB device
 * @param endpoint Endpoint number
 * @param data Pointer to data buffer
 * @param length Buffer length
 * @param callback Completion callback
 * @param user_data Callback user data
 * @return bool Success status
 */
bool usb_bulk_receive(usb_device_t *device, uint8_t endpoint, uint8_t *data,
                     uint16_t length, usb_transfer_callback_t callback, void *user_data);

/**
 * @brief Send interrupt data
 * @param device Pointer to USB device
 * @param endpoint Endpoint number
 * @param data Pointer to data buffer
 * @param length Data length
 * @param callback Completion callback
 * @param user_data Callback user data
 * @return bool Success status
 */
bool usb_interrupt_send(usb_device_t *device, uint8_t endpoint, const uint8_t *data,
                       uint16_t length, usb_transfer_callback_t callback, void *user_data);

// Control Transfer Functions
/**
 * @brief Process control request
 * @param device Pointer to USB device
 * @param setup Pointer to setup request structure
 * @param data Pointer to data buffer
 * @param length Data length
 * @return bool Success status
 */
bool usb_control_request(usb_device_t *device, usb_setup_t *setup, 
                        uint8_t *data, uint16_t *length);

/**
 * @brief Send control response
 * @param device Pointer to USB device
 * @param data Pointer to response data
 * @param length Response length
 * @return bool Success status
 */
bool usb_control_response(usb_device_t *device, const uint8_t *data, uint16_t length);

/**
 * @brief Handle vendor-specific request
 * @param device Pointer to USB device
 * @param request Vendor request code
 * @param value Request value
 * @param index Request index
 * @param data Pointer to data buffer
 * @param length Data length
 * @return bool Success status
 */
bool usb_vendor_request(usb_device_t *device, uint8_t request, uint16_t value,
                       uint16_t index, uint8_t *data, uint16_t *length);

// Enumeration Functions
/**
 * @brief Process USB setup packet
 * @param setup_data Raw setup packet data
 * @param length Packet length
 * @return bool Success status
 */
bool usb_process_setup_packet(const uint8_t *setup_data, uint16_t length);

/**
 * @brief Handle device reset
 */
void usb_device_reset(void);

/**
 * @brief Handle device suspend
 */
void usb_device_suspend(void);

/**
 * @brief Handle device resume
 */
void usb_device_resume(void);

// String Descriptors
/**
 * @brief Get string descriptor
 * @param device Pointer to USB device
 * @param index String index
 * @param language_id Language ID
 * @param descriptor Pointer to descriptor buffer
 * @param length Pointer to descriptor length
 * @return bool Success status
 */
bool usb_get_string_descriptor(usb_device_t *device, uint8_t index, uint16_t language_id,
                              uint8_t *descriptor, uint16_t *length);

/**
 * @brief Get manufacturer string
 * @param device Pointer to USB device
 * @param descriptor Pointer to descriptor buffer
 * @param length Pointer to descriptor length
 * @return bool Success status
 */
bool usb_get_manufacturer_string(usb_device_t *device, uint8_t *descriptor, uint16_t *length);

/**
 * @brief Get product string
 * @param device Pointer to USB device
 * @param descriptor Pointer to descriptor buffer
 * @param length Pointer to descriptor length
 * @return bool Success status
 */
bool usb_get_product_string(usb_device_t *device, uint8_t *descriptor, uint16_t *length);

/**
 * @brief Get serial number string
 * @param device Pointer to USB device
 * @param descriptor Pointer to descriptor buffer
 * @param length Pointer to descriptor length
 * @return bool Success status
 */
bool usb_get_serial_number_string(usb_device_t *device, uint8_t *descriptor, uint16_t *length);

// Power Management
/**
 * @brief Check if device is self-powered
 * @return bool Self-powered status
 */
bool usb_is_self_powered(void);

/**
 * @brief Enable/disable remote wakeup
 * @param enable Enable flag
 */
void usb_set_remote_wakeup(bool enable);

/**
 * @brief Check remote wakeup capability
 * @return bool Remote wakeup enabled
 */
bool usb_get_remote_wakeup(void);

// Callback Management
/**
 * @brief Set device callback
 * @param device Pointer to USB device
 * @param event Event type
 * @param callback Callback function
 * @param user_data Callback user data
 */
void usb_set_device_callback(usb_device_t *device, usb_device_callback_t callback, void *user_data);

/**
 * @brief Set transfer callback for endpoint
 * @param device Pointer to USB device
 * @param endpoint Endpoint number
 * @param callback Transfer callback
 * @param user_data Callback user data
 */
void usb_set_transfer_callback(usb_device_t *device, uint8_t endpoint,
                              usb_transfer_callback_t callback, void *user_data);

// Error Handling
/**
 * @brief Get last USB error
 * @return uint32_t Error code
 */
uint32_t usb_get_last_error(void);

/**
 * @brief Clear last USB error
 */
void usb_clear_last_error(void);

/**
 * @brief Check if USB error occurred
 * @return bool Error status
 */
bool usb_has_error(void);

// Utility Functions
/**
 * @brief Calculate CRC16 for USB data
 * @param data Pointer to data
 * @param length Data length
 * @return uint16_t CRC16 value
 */
uint16_t usb_crc16(const uint8_t *data, uint16_t length);

/**
 * @brief Parse USB address from setup packet
 * @param setup Pointer to setup packet
 * @return uint8_t Device address
 */
uint8_t usb_parse_address(const usb_setup_t *setup);

/**
 * @brief Check if request is for interface
 * @param setup Pointer to setup packet
 * @return bool Interface request flag
 */
bool usb_is_interface_request(const usb_setup_t *setup);

/**
 * @brief Check if request is for endpoint
 * @param setup Pointer to setup packet
 * @return bool Endpoint request flag
 */
bool usb_is_endpoint_request(const usb_setup_t *setup);

/**
 * @brief Check if request is vendor-specific
 * @param setup Pointer to setup packet
 * @return bool Vendor request flag
 */
bool usb_is_vendor_request(const usb_setup_t *setup);

// Interrupt Service Routines
/**
 * @brief USB interrupt service routine
 */
void __attribute__((interrupt, no_auto_psv)) _USB1Interrupt(void);

/**
 * @brief USB reset interrupt service routine
 */
void __attribute__((interrupt, no_auto_psv)) _USBRestartInterrupt(void);

/**
 * @brief USB endpoint interrupt service routine
 */
void __attribute__((interrupt, no_auto_psv)) _USBEndpointInterrupt(void);

#endif /* USB_DEVICE_H */
