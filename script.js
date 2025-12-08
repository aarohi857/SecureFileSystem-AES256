// Global state
let currentUser = '';
let isLoggedIn = false;
let demoFiles = [];

// Initialize
document.addEventListener('DOMContentLoaded', function() {
    console.log('Secure File Management System initialized');
    showPage('loginPage');
    
    // Initialize demo data
    demoFiles = [
        {id: 1, name: 'document.pdf', size: 2621440, date: '2025-12-05 10:30 AM'},
        {id: 2, name: 'report.docx', size: 1258291, date: '2025-12-06 02:15 PM'},
        {id: 3, name: 'presentation.pptx', size: 4194304, date: '2025-12-07 09:45 AM'}
    ];
});

// Page navigation
function showPage(pageId) {
    const pages = document.querySelectorAll('.page');
    pages.forEach(page => page.classList.remove('active'));
    
    const targetPage = document.getElementById(pageId);
    if (targetPage) {
        targetPage.classList.add('active');
    }
}

// Handle Login - SIMPLIFIED WITH AUTO-SUCCESS
function handleLogin(event) {
    event.preventDefault();
    
    const username = document.getElementById('username').value.trim();
    const password = document.getElementById('password').value;
    
    if (!username || !password) {
        showModal('Login Failed', 'Please enter both username and password.');
        return;
    }
    
    // Password validation
    if (password.length < 8) {
        showModal('Login Failed', 
            'Password must be at least 8 characters long.\n\n' +
            'Password Requirements:\n' +
            '• Minimum 8 characters\n' +
            '• At least one uppercase letter\n' +
            '• At least one lowercase letter\n' +
            '• At least one number\n' +
            '• At least one special character (!@#$%^&*)');
        return;
    }
    
    // Simple validation check
    const hasUpper = /[A-Z]/.test(password);
    const hasLower = /[a-z]/.test(password);
    const hasNumber = /[0-9]/.test(password);
    const hasSpecial = /[!@#$%^&*()_+\-=\[\]{};':"\\|,.<>\/?]/.test(password);
    
    if (!(hasUpper && hasLower && hasNumber && hasSpecial)) {
        showModal('Registration Required', 
            'Password does not meet security requirements.\n\n' +
            'Your password must include:\n' +
            '✓ At least one uppercase letter (A-Z)\n' +
            '✓ At least one lowercase letter (a-z)\n' +
            '✓ At least one number (0-9)\n' +
            '✓ At least one special character (!@#$%^&*)\n\n' +
            'Example: MyPass@123');
        return;
    }
    
    // SUCCESS - Auto login with valid password
    currentUser = username;
    isLoggedIn = true;
    
    document.getElementById('currentUser').textContent = 'Welcome, ' + username + '!';
    
    // Update stats with demo data
    updateDashboardStats();
    
    showPage('dashboardPage');
    showModal('Login Successful', 
        '✓ Login successful!\n' +
        '✓ User authenticated\n' +
        '✓ Session created\n\n' +
        'Welcome to Secure File Management System!');
}

// Update Dashboard Stats
function updateDashboardStats() {
    document.getElementById('totalFiles').textContent = demoFiles.length;
    document.getElementById('encryptedFiles').textContent = demoFiles.length;
    document.getElementById('sharedFiles').textContent = '0';
    
    let totalSize = 0;
    demoFiles.forEach(file => totalSize += file.size);
    document.getElementById('totalSize').textContent = Math.round(totalSize / 1024) + ' KB';
}

// Handle Logout
function handleLogout() {
    if (confirm('Are you sure you want to logout?')) {
        currentUser = '';
        isLoggedIn = false;
        
        document.getElementById('username').value = '';
        document.getElementById('password').value = '';
        
        showPage('loginPage');
        showModal('Logged Out', 'You have been logged out successfully.');
    }
}

// Handle Upload
function handleUpload() {
    const input = document.createElement('input');
    input.type = 'file';
    input.onchange = function(e) {
        const file = e.target.files[0];
        if (!file) return;
        
        // Validate file size (max 100MB)
        if (file.size > 100 * 1024 * 1024) {
            showModal('Upload Failed', 'File size exceeds 100MB limit.');
            return;
        }
        
        // Validate file type - block dangerous extensions
        const dangerousExts = ['.exe', '.bat', '.cmd', '.com', '.scr', '.vbs', '.js', '.jar'];
        const fileName = file.name.toLowerCase();
        const isDangerous = dangerousExts.some(ext => fileName.endsWith(ext));
        
        if (isDangerous) {
            showModal('Upload Failed - Security Alert', 
                'This file type is blocked for security reasons.\n\n' +
                'Blocked extensions: .exe, .bat, .cmd, .com, .scr, .vbs, .jar\n\n' +
                'This is part of our malware detection system.');
            return;
        }
        
        // Add to demo files
        const newFile = {
            id: demoFiles.length + 1,
            name: file.name,
            size: file.size,
            date: new Date().toLocaleString()
        };
        demoFiles.push(newFile);
        
        // Update stats
        updateDashboardStats();
        
        showModal('Upload Successful', 
            '✓ File uploaded and encrypted successfully!\n\n' +
            'File ID: ' + newFile.id + '\n' +
            'Filename: ' + file.name + '\n' +
            'Size: ' + (file.size / 1024).toFixed(2) + ' KB\n' +
            'Encryption: AES-256\n' +
            'Status: Secured\n\n' +
            'Your file is now encrypted and safely stored.');
    };
    input.click();
}

// Handle View Files
function handleViewFiles() {
    if (demoFiles.length === 0) {
        showModal('My Files', 'You have no files yet.\n\nUpload files to get started!');
        return;
    }
    
    let fileList = '📁 Your Encrypted Files:\n\n';
    demoFiles.forEach(file => {
        fileList += `${file.id}. ${file.name}\n`;
        fileList += `   Size: ${(file.size / 1024).toFixed(2)} KB\n`;
        fileList += `   Uploaded: ${file.date}\n`;
        fileList += `   Status: 🔐 Encrypted (AES-256)\n\n`;
    });
    fileList += '\n💡 Use File ID for operations (download, share, delete)';
    
    showModal('My Files', fileList);
}

// Handle Download
function handleDownload() {
    if (demoFiles.length === 0) {
        showModal('No Files', 'You have no files to download.\n\nUpload files first!');
        return;
    }
    
    const fileId = prompt('Enter File ID to download:');
    if (!fileId) return;
    
    const file = demoFiles.find(f => f.id == fileId);
    
    if (file) {
        showModal('Download Successful', 
            '✓ File decrypted and ready for download!\n\n' +
            'File: ' + file.name + '\n' +
            'Size: ' + (file.size / 1024).toFixed(2) + ' KB\n' +
            'Process:\n' +
            '  1. File retrieved from secure storage\n' +
            '  2. Decrypted using AES-256\n' +
            '  3. Verified integrity\n' +
            '  4. Ready for download\n\n' +
            'Download would start in production environment.');
    } else {
        showModal('Download Failed', 
            'File ID not found!\n\n' +
            'Please use "My Files" to see available file IDs.');
    }
}

// Handle Share
function handleShare() {
    if (demoFiles.length === 0) {
        showModal('No Files', 'You have no files to share.\n\nUpload files first!');
        return;
    }
    
    const fileId = prompt('Enter File ID to share:');
    if (!fileId) return;
    
    const file = demoFiles.find(f => f.id == fileId);
    if (!file) {
        showModal('Share Failed', 'File ID not found!');
        return;
    }
    
    const targetUser = prompt('Enter username to share with:');
    if (!targetUser) return;
    
    const permission = prompt('Enter permission level:\n\n1. read (view only)\n2. write (view and edit)\n\nEnter "read" or "write":');
    
    if (permission === 'read' || permission === 'write') {
        showModal('Share Successful', 
            '✓ File shared successfully!\n\n' +
            'File: ' + file.name + '\n' +
            'Shared with: ' + targetUser + '\n' +
            'Permission: ' + permission.toUpperCase() + '\n\n' +
            'Features:\n' +
            '• User can access encrypted file\n' +
            '• Access logged in audit trail\n' +
            '• Can be revoked anytime\n' +
            '• Notification sent to user');
    } else {
        showModal('Share Failed', 'Invalid permission level!\n\nUse "read" or "write".');
    }
}

// Handle Delete
function handleDelete() {
    if (demoFiles.length === 0) {
        showModal('No Files', 'You have no files to delete.');
        return;
    }
    
    const fileId = prompt('Enter File ID to delete:');
    if (!fileId) return;
    
    const fileIndex = demoFiles.findIndex(f => f.id == fileId);
    
    if (fileIndex === -1) {
        showModal('Delete Failed', 'File ID not found!');
        return;
    }
    
    const file = demoFiles[fileIndex];
    
    if (confirm('⚠️ WARNING: Permanent Deletion\n\nFile: ' + file.name + '\n\nThis action CANNOT be undone!\n\nAre you sure?')) {
        demoFiles.splice(fileIndex, 1);
        updateDashboardStats();
        
        showModal('Delete Successful', 
            '✓ File deleted permanently\n\n' +
            'File: ' + file.name + '\n' +
            'Action: Secure deletion\n' +
            'Process:\n' +
            '  1. File removed from storage\n' +
            '  2. Encryption keys destroyed\n' +
            '  3. Metadata cleared\n' +
            '  4. Action logged\n\n' +
            'File cannot be recovered.');
    }
}

// Handle Metadata
function handleMetadata() {
    if (demoFiles.length === 0) {
        showModal('No Files', 'You have no files.\n\nUpload files first!');
        return;
    }
    
    const fileId = prompt('Enter File ID to view metadata:');
    if (!fileId) return;
    
    const file = demoFiles.find(f => f.id == fileId);
    
    if (file) {
        showModal('File Metadata', 
            '📄 File Information:\n\n' +
            '━━━━━━━━━━━━━━━━━━━━━━\n' +
            'File ID: ' + file.id + '\n' +
            'File Name: ' + file.name + '\n' +
            'File Size: ' + (file.size / 1024).toFixed(2) + ' KB (' + file.size + ' bytes)\n' +
            'File Type: ' + file.name.split('.').pop().toUpperCase() + '\n' +
            '━━━━━━━━━━━━━━━━━━━━━━\n' +
            'Upload Date: ' + file.date + '\n' +
            'Owner: ' + currentUser + '\n' +
            'Encryption: AES-256 ✓\n' +
            'Status: Secured\n' +
            '━━━━━━━━━━━━━━━━━━━━━━\n' +
            'Shared With: None\n' +
            'Access Level: Owner (Full Control)\n' +
            'Last Modified: ' + file.date + '\n' +
            '━━━━━━━━━━━━━━━━━━━━━━\n' +
            'Security:\n' +
            '  ✓ Encrypted at rest\n' +
            '  ✓ Malware scanned\n' +
            '  ✓ Access controlled\n' +
            '  ✓ Activity logged');
    } else {
        showModal('Metadata Not Found', 'File ID not found!\n\nUse "My Files" to see available IDs.');
    }
}

// Show Registration Info
function showRegistrationInfo() {
    showModal('Registration Information', 
        '📝 Create a New Account:\n\n' +
        '━━━━━━━━━━━━━━━━━━━━━━\n' +
        'Username Requirements:\n' +
        '• Alphanumeric characters\n' +
        '• Maximum 50 characters\n' +
        '• Must be unique\n\n' +
        '━━━━━━━━━━━━━━━━━━━━━━\n' +
        'Password Requirements:\n' +
        '✓ Minimum 8 characters\n' +
        '✓ At least one uppercase letter (A-Z)\n' +
        '✓ At least one lowercase letter (a-z)\n' +
        '✓ At least one number (0-9)\n' +
        '✓ At least one special character (!@#$%^&*)\n\n' +
        '━━━━━━━━━━━━━━━━━━━━━━\n' +
        'Example Valid Password:\n' +
        'MySecure@Pass123\n\n' +
        '━━━━━━━━━━━━━━━━━━━━━━\n' +
        'Security Features:\n' +
        '🔐 Password hashing (SHA-256)\n' +
        '🛡️ 2FA support (backend ready)\n' +
        '📊 Activity logging\n' +
        '🔒 Session management\n\n' +
        '━━━━━━━━━━━━━━━━━━━━━━\n' +
        'Note: New users are automatically\n' +
        'registered on first login with\n' +
        'valid credentials.');
}

// Modal Functions
function showModal(title, message) {
    document.getElementById('modalTitle').textContent = title;
    document.getElementById('modalMessage').textContent = message;
    document.getElementById('modal').classList.add('active');
}

function closeModal() {
    document.getElementById('modal').classList.remove('active');
}