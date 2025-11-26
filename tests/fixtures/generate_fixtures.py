import numpy as np

# Generate sine_waveforms.npy - reference signals for frequency analysis
sampling_rate = 105e6  # 105 MSPS
frequencies = [10e6, 20e6, 1e6]  # Different test frequencies
duration = 1024 / sampling_rate  # 1024 samples
time = np.arange(0, duration, 1/sampling_rate)

waveforms = {}
for freq in frequencies:
    waveforms[f'{int(freq/1e6)}Mhz'] = (np.sin(2 * np.pi * freq * time) * 2047).astype(np.int16)  # 11-bit range

np.save('sine_waveforms.npy', waveforms)
print('Generated sine_waveforms.npy')

# Generate ethernet_frames.pcap simulation - simple Ethernet frame data
import struct

# Ethernet frame: DST MAC + SRC MAC + Type + Payload
dst_mac = b'\xFF\xFF\xFF\xFF\xFF\xFF'  # Broadcast
src_mac = b'\x02\x00\x00\x00\x00\x01'  # Local MAC from MAC module
eth_type = b'\x08\x00'  # IPv4
payload = b'Hello SDR!'  # Sample payload

frame = dst_mac + src_mac + eth_type + payload
padding = b'\x00' * (64 - len(frame))  # Minimum Ethernet frame size
frame += padding

# Simple PCAP header simulation (binary file)
pcap_data = frame
with open('ethernet_frames.pcap', 'wb') as f:
    f.write(pcap_data)

print('Generated ethernet_frames.pcap')

# Generate ddc_test_signals.bin - Digital downconversion test vectors
adc_samples = np.random.randint(-2048, 2047, 4096, dtype=np.int16)  # 12-bit ADC-like samples
with open('ddc_test_signals.bin', 'wb') as f:
    for sample in adc_samples:
        f.write(sample.tobytes())

print('Generated ddc_test_signals.bin')
