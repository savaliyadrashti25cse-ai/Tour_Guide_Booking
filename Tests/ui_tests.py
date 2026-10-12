"""Checks real console interactions. Requires only Python's standard library."""
from pathlib import Path
import os
import subprocess
import tempfile

root = Path(__file__).resolve().parents[1]
exe = root / 'Executable' / ('Project.exe' if os.name == 'nt' else 'Project')
with tempfile.TemporaryDirectory(prefix='cpp45_ui_') as directory:
    def run(lines):
        result = subprocess.run([str(exe), '--data-dir', directory], input='\n'.join(lines)+'\n', text=True, capture_output=True, timeout=20)
        assert result.returncode == 0, result.stderr
        return result.stdout
    seed = ['abc','1','1','Alex Demo','9000000001','Ahmedabad',
            '1','2','Guide Demo','9000000002','Hindi','1000','1',
            '1','3','City Tour','Ahmedabad','2026-10-20','1','4','500',
            '6','1','1','1','1','2','y','9']
    output = run(seed)
    assert 'Invalid input. Enter an integer' in output
    assert 'Booking created successfully.' in output and 'Total: INR 2000.00' in output
    print('PASS | UI01 Invalid menu input and valid booking creation')
    output = run(['2','5','3','1','1','1','9'])
    assert 'Tourist #1 | Alex Demo' in output and 'Booking #1' in output
    print('PASS | UI02 Display, ID search and restart persistence')
    output = run(['3','1','2','Alex','4','1','1','Alex Updated','9000000001','Surat','9'])
    assert 'Tourist #1 | Alex Demo' in output and 'Record updated and saved.' in output
    print('PASS | UI03 Text search and tourist update')
    output = run(['5','1','1','y','6','2','1','y','5','4','1','y','5','1','1','y','9'])
    assert 'Record is referenced by a booking' in output
    assert 'Booking cancelled and saved.' in output
    assert output.count('Record deleted and saved.') == 2
    print('PASS | UI04 Protected deletion, cancellation and permitted deletion')
    output = run(['7','8','2','y','9'])
    assert 'Confirmed bookings: 0' in output and 'Data loaded successfully.' in output
    assert (Path(directory)/'report.txt').is_file()
    print('PASS | UI05 Report export and explicit reload')
    output = run(['1','3','Bad Date','City','2026-02-30','1','5','100','9'])
    assert 'Error: Day must be between' in output
    print('PASS | UI06 Invalid date rejected through the menu')
print('RESULT | 6 console scenarios passed, 0 failed')
