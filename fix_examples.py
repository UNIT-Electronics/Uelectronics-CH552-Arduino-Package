#!/usr/bin/env python3
"""Remove USER_USB_RAM checks from example sketches."""

import os
import glob

examples_dir = "ch55x/libraries/devlab_ch55x/examples"
fixed_count = 0

for ino_file in glob.glob(f"{examples_dir}/**/*.ino", recursive=True):
    with open(ino_file, 'r') as f:
        lines = f.readlines()
    
    # Remove the #ifndef user_usb_ram block
    new_lines = []
    skip = False
    for line in lines:
        if line.strip() == "#ifndef user_usb_ram":
            skip = True
            continue
        if skip and line.strip() == "#endif":
            skip = False
            continue
        if not skip:
            new_lines.append(line)
    
    # Write back if changed
    if len(new_lines) != len(lines):
        with open(ino_file, 'w') as f:
            f.writelines(new_lines)
        print(f"✓ Fixed: {ino_file}")
        fixed_count += 1

print(f"\n✅ Fixed {fixed_count} files")
