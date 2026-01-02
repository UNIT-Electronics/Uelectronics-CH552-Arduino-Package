#!/usr/bin/env python3
"""Fix include statement case to match actual filenames."""

import os
import re
from pathlib import Path

# Map incorrect includes to correct filenames
INCLUDE_FIXES = {
    r'#include\s*[<"]arduino\.h[>"]': '#include "Arduino.h"',
    r'#include\s*[<"]hardwareserial\.h[>"]': '#include "HardwareSerial.h"',
    r'#include\s*[<"]usbhandler\.h[>"]': '#include "USBhandler.h"',
    r'#include\s*[<"]usbconstant\.h[>"]': '#include "USBconstant.h"',
}

def fix_includes_in_file(filepath):
    """Fix include statements in a single file."""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
    except:
        return False
    
    original_content = content
    
    # Apply all fixes
    for pattern, replacement in INCLUDE_FIXES.items():
        content = re.sub(pattern, replacement, content, flags=re.IGNORECASE)
    
    # Only write if changed
    if content != original_content:
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(content)
        return True
    
    return False

def main():
    """Process all C files in the ch55x core directory."""
    core_dir = Path('/media/mr/firmware/github-mx/support_devlab/Uelectronics-CH552-Arduino-Package/ch55x/cores/ch55x')
    
    fixed_count = 0
    
    # Process all .c files recursively
    for c_file in core_dir.rglob('*.c'):
        if fix_includes_in_file(str(c_file)):
            print(f"✓ Fixed: {c_file.relative_to(core_dir)}")
            fixed_count += 1
    
    print(f"\n✅ Fixed {fixed_count} files")

if __name__ == '__main__':
    main()
