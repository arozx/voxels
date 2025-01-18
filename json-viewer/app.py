import json
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path
import argparse
import signal
import sys
import atexit

def signal_handler(sig, frame):
    print('\nExiting gracefully...')
    plt.close('all')
    sys.exit(0)

def cleanup():
    plt.close('all')

# Register handlers
signal.signal(signal.SIGINT, signal_handler)
signal.signal(signal.SIGTERM, signal_handler)
atexit.register(cleanup)

def load_profile_data(filepath):
    with open(filepath, 'r') as f:
        data = json.load(f)
    
    profiles = []
    for profile in data['profiles']:
        profiles.append({
            'name': profile['name'],
            'samples': profile['samples'],
            'calls': int(profile['calls']),
            'avg_ms': float(profile['averageMs']),
            'min_ms': float(profile['minMs']),
            'max_ms': float(profile['maxMs'])
        })
    
    # Create DataFrame with expanded samples
    rows = []
    for p in profiles:
        for sample in p['samples']:
            rows.append({
                'name': p['name'],
                'duration': sample,
                'calls': p['calls'],
                'avg_ms': p['avg_ms']
            })
    
    return pd.DataFrame(rows)

def visualize_profile_data(df, output_file=None):
    plt.style.use('ggplot')
    
    # Calculate statistics
    stats = df.groupby('name').agg({
        'duration': ['mean', 'std', 'min', 'max', 'count'],
        'calls': 'first',
        'avg_ms': 'first'
    }).round(3)
    
    # Calculate total impact
    stats['total_ms'] = stats[('duration', 'mean')] * stats[('calls', 'first')]
    stats = stats.sort_values('total_ms', ascending=True)
    
    # Create figure with adjusted margins
    plt.figure(figsize=(12, 8))
    plt.subplots_adjust(left=0.2, right=0.9, bottom=0.15, top=0.95)
    
    # Plot 1: Total time impact
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 10))
    fig.subplots_adjust(left=0.25, right=0.95, bottom=0.1, top=0.95, hspace=0.3)
    
    colors = plt.cm.RdYlGn_r(np.linspace(0, 1, len(stats.index)))
    bars = ax1.barh(stats.index, stats['total_ms'], color=colors)
    ax1.set_title('Total Time Impact')
    ax1.set_xlabel('Total Time (ms)')
    ax1.grid(True, axis='x')
    
    # Plot 2: Duration distribution
    box_data = [df[df['name'] == name]['duration'] for name in stats.index]
    ax2.boxplot(box_data, 
                vert=False,
                labels=stats.index,
                flierprops={'alpha': 0.5, 'markersize': 4})
    ax2.set_title('Duration Distribution')
    ax2.set_xlabel('Duration (ms)')
    ax2.grid(True, axis='x')
    
    if output_file:
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
    else:
        plt.show()
    
    # Print statistics
    print("\nFunction Statistics:")
    for name in stats.index:
        row = stats.loc[name]
        print(f"\n{name}:")
        print(f"  Calls: {row[('calls', 'first')]:,}")
        print(f"  Samples: {row[('duration', 'count')]:,}")
        print(f"  Average: {row[('duration', 'mean')]:.3f}ms")
        print(f"  Std Dev: {row[('duration', 'std')]:.3f}ms")
        print(f"  Min/Max: {row[('duration', 'min')]:.3f}ms / {row[('duration', 'max')]:.3f}ms")
        print(f"  Total Time: {row['total_ms']:.1f}ms")

def main():
    try:
        parser = argparse.ArgumentParser(description='Visualize profiler data')
        parser.add_argument('input', nargs='?', default='profile_results.json', 
            help='Input JSON file (default: profile_results.json)')
        parser.add_argument('-o', '--output', help='Output image file (optional)')
        args = parser.parse_args()
        
        profile_path = Path(args.input)
        
        if not profile_path.exists():
            print(f"Error: No profile data found at {profile_path}")
            return 1
            
        df = load_profile_data(profile_path)
        visualize_profile_data(df, args.output)
        return 0
        
    except KeyboardInterrupt:
        print('\nOperation cancelled by user')
        return 1
    except Exception as e:
        print(f"Error processing profile data: {e}")
        return 1
    finally:
        plt.close('all')

if __name__ == '__main__':
    sys.exit(main())