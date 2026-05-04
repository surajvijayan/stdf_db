#!/usr/bin/env python3
"""
Fix C++ code indentation so opening and closing braces align at the same column.
Implements Allman style: opening brace on new line, same indentation as closing.
"""

import sys
import re

def format_code(content):
    """Format C++ code with proper brace alignment."""
    lines = content.split('\n')
    formatted = []
    indent_level = 0
    indent_str = "    "  # 4 spaces
    
    i = 0
    while i < len(lines):
        line = lines[i]
        stripped = line.lstrip()
        
        # Skip empty lines and comments
        if not stripped or stripped.startswith('//'):
            formatted.append(line)
            i += 1
            continue
        
        # Count closing braces at start of line and decrease indent
        closing_braces = 0
        for char in stripped:
            if char == '}':
                closing_braces += 1
            else:
                break
        
        if closing_braces > 0:
            indent_level = max(0, indent_level - closing_braces)
        
        # Apply current indent to non-empty lines
        if stripped:
            formatted.append(indent_str * indent_level + stripped)
        else:
            formatted.append('')
        
        # Count opening braces and increase indent
        opening_braces = stripped.count('{')
        closing_braces_end = stripped.count('}')
        net_braces = opening_braces - closing_braces_end
        indent_level += net_braces
        indent_level = max(0, indent_level)
        
        i += 1
    
    return '\n'.join(formatted)

def main():
    """Process all C++ files in current directory."""
    import glob
    
    cpp_files = glob.glob('*.cpp') + glob.glob('*.h')
    
    for filename in cpp_files:
        if filename == 'format_braces.py':
            continue
        
        print(f"Formatting {filename}...", file=sys.stderr)
        
        try:
            with open(filename, 'r', encoding='utf-8') as f:
                content = f.read()
            
            formatted = format_code(content)
            
            with open(filename, 'w', encoding='utf-8') as f:
                f.write(formatted)
            
            print(f"  ✓ {filename} formatted", file=sys.stderr)
        except Exception as e:
            print(f"  ✗ Error processing {filename}: {e}", file=sys.stderr)

if __name__ == '__main__':
    main()
