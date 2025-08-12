#!/usr/bin/env python3
"""
Script to create new Kvantum themes by copying and modifying existing themes.
Uses JSON configuration files for flexible theme creation.
"""

import os
import shutil
import re
import json
import argparse
from pathlib import Path

def copy_theme_directory(source_theme, dest_theme, base_dir):
    """Copy theme directory from source to destination."""
    source_path = Path(base_dir) / "bin/themes/kvantum" / source_theme
    dest_path = Path(base_dir) / "bin/themes/kvantum" / dest_theme
    
    if not source_path.exists():
        raise FileNotFoundError(f"Source theme directory not found: {source_path}")
    
    if dest_path.exists():
        print(f"Destination directory exists, removing: {dest_path}")
        shutil.rmtree(dest_path)
    
    print(f"Copying {source_path} -> {dest_path}")
    shutil.copytree(source_path, dest_path)
    
    return dest_path

def rename_theme_files(theme_dir, old_name, new_name):
    """Rename theme files from old_name to new_name."""
    theme_path = Path(theme_dir)
    
    for old_file in theme_path.glob(f"{old_name}.*"):
        new_file = old_file.parent / f"{new_name}{old_file.suffix}"
        print(f"Renaming {old_file.name} -> {new_file.name}")
        old_file.rename(new_file)

def replace_colors_in_file(file_path, color_map, case_sensitive=False):
    """Replace colors in a text file using the provided color mapping."""
    if not color_map:
        return
        
    print(f"Processing color replacements in: {file_path.name}")
    
    try:
        # Read entire file content
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Track replacements made and not found
        replacements_made = {}
        colors_not_found = []
        
        # Apply color replacements
        for old_color, new_color in color_map.items():
            found = False
            
            if case_sensitive:
                # Case sensitive replacement
                if old_color in content:
                    count = content.count(old_color)
                    content = content.replace(old_color, new_color)
                    replacements_made[old_color] = count
                    found = True
            else:
                # Case insensitive search but preserve the original case in replacement
                pattern = re.compile(re.escape(old_color), re.IGNORECASE)
                matches = pattern.findall(content)
                if matches:
                    # Replace each match while preserving its original case format
                    def case_preserving_replace(match):
                        original_match = match.group(0)
                        # If original is all uppercase, make new color uppercase
                        if original_match.isupper():
                            return new_color.upper()
                        # If original is all lowercase, make new color lowercase  
                        elif original_match.islower():
                            return new_color.lower()
                        # If mixed case, preserve the pattern from original
                        else:
                            result = ""
                            for i, char in enumerate(new_color):
                                if i < len(original_match):
                                    if original_match[i].isupper():
                                        result += char.upper()
                                    else:
                                        result += char.lower()
                                else:
                                    result += char
                            return result
                    
                    # Count matches first
                    replacements_made[old_color] = len(matches)
                    # Do the replacement with case preservation
                    content = pattern.sub(case_preserving_replace, content)
                    found = True
            
            if not found:
                colors_not_found.append(old_color)
        
        # Write back to file
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(content)
        
        # Report replacements
        if replacements_made:
            print(f"  ✅ Replacements made:")
            for old, count in replacements_made.items():
                print(f"    {old} -> {color_map[old]} ({count} times)")
        
        # Report colors not found as warnings
        if colors_not_found:
            print(f"  ⚠️  Colors not found in {file_path.name}:")
            for color in colors_not_found:
                print(f"    {color} (no replacement needed)")
        
        if not replacements_made and not colors_not_found:
            print(f"  ✅ No color replacements needed in {file_path.name}")
            
    except Exception as e:
        print(f"❌ Error processing {file_path}: {e}")
        raise

def update_kvconfig_settings(kvconfig_path, config):
    """Update settings in the .kvconfig file."""
    print(f"Updating settings in: {kvconfig_path.name}")
    
    with open(kvconfig_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Update metadata
    if 'author' in config:
        content = re.sub(r'^author=.*$', f'author={config["author"]}', content, flags=re.MULTILINE)
    
    if 'comment' in config:
        content = re.sub(r'^comment=.*$', f'comment={config["comment"]}', content, flags=re.MULTILINE)
    
    # Update animate_states setting if specified
    if 'animate_states' in config:
        animate_value = str(config['animate_states']).lower()
        if re.search(r'^animate_states=', content, re.MULTILINE):
            content = re.sub(r'^animate_states=.*$', f'animate_states={animate_value}', content, flags=re.MULTILINE)
        else:
            # Add animate_states to [%General] section if it doesn't exist
            general_pattern = r'(\[%General\])'
            if re.search(general_pattern, content):
                content = re.sub(general_pattern, f'\\1\\nanimate_states={animate_value}', content)
    
    with open(kvconfig_path, 'w', encoding='utf-8') as f:
        f.write(content)

def update_qrc_file(qrc_path, old_theme, new_theme):
    """Update the .qrc file with new theme name."""
    print(f"Updating QRC file: {qrc_path.name}")
    
    with open(qrc_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Replace theme name in resource prefix and file references
    content = content.replace(f"/{old_theme}", f"/{new_theme}")
    content = content.replace(f"{old_theme}.", f"{new_theme}.")
    
    with open(qrc_path, 'w', encoding='utf-8') as f:
        f.write(content)

def substitute_variables(config):
    """Replace variable references in config with actual values."""
    if 'variables' not in config:
        return config
    
    variables = config['variables']
    config_str = json.dumps(config)
    
    # Replace ${variable_name} patterns with actual values
    for var_name, var_value in variables.items():
        config_str = config_str.replace(f"${{{var_name}}}", var_value)
    
    return json.loads(config_str)

def create_theme_from_config(config_file, base_dir="."):
    """Main function to create a theme from JSON configuration."""
    # Load configuration
    with open(config_file, 'r', encoding='utf-8') as f:
        config = json.load(f)
    
    # Process variable substitutions
    config = substitute_variables(config)
    
    source_theme = config['source_theme']
    new_theme = config['destination_theme']
    
    print(f"Creating new theme '{new_theme}' based on '{source_theme}'")
    print(f"Configuration: {config_file}")
    
    # Copy theme directory
    dest_dir = copy_theme_directory(source_theme, new_theme, base_dir)
    
    # Rename files
    rename_theme_files(dest_dir, source_theme, new_theme)
    
    # Process files
    kvconfig_file = dest_dir / f"{new_theme}.kvconfig"
    svg_file = dest_dir / f"{new_theme}.svg"
    qrc_file = dest_dir / f"{new_theme}.qrc"
    
    # Update kvconfig file
    if kvconfig_file.exists():
        # Replace colors first
        kvconfig_colors = config.get('kvconfig_colors', {})
        if kvconfig_colors:
            replace_colors_in_file(kvconfig_file, kvconfig_colors, case_sensitive=True)
        
        # Update settings (author, comment, animate_states)
        update_kvconfig_settings(kvconfig_file, config)
    
    # Replace colors in SVG file
    if svg_file.exists():
        svg_colors = config.get('svg_colors', {})
        if svg_colors:
            replace_colors_in_file(svg_file, svg_colors, case_sensitive=False)
    
    # Update QRC file
    if qrc_file.exists():
        update_qrc_file(qrc_file, source_theme, new_theme)
    
    print(f"\n✅ Theme '{new_theme}' created successfully!")
    print(f"📁 Location: {dest_dir}")
    
    return dest_dir

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Create new Kvantum themes from JSON configuration")
    parser.add_argument("config", help="JSON configuration file (e.g., FTICerise.json)")
    parser.add_argument("--base-dir", default="../..", help="Base directory (default: two levels up from script location)")
    
    args = parser.parse_args()
    
    try:
        create_theme_from_config(args.config, args.base_dir)
    except FileNotFoundError as e:
        print(f"❌ Error: {e}")
    except json.JSONDecodeError as e:
        print(f"❌ JSON Error: {e}")
    except Exception as e:
        print(f"❌ Unexpected error: {e}")
        raise