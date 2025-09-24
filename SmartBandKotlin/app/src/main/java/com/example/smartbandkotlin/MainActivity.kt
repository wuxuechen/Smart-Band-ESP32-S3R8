package com.example.smartbandkotlin

import android.Manifest
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothManager
import android.bluetooth.le.ScanCallback
import android.bluetooth.le.ScanResult
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.content.pm.PackageManager
import android.net.wifi.ScanResult as WifiScanResult
import android.net.wifi.WifiManager
import android.os.Build
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.util.Log
import android.widget.*
import androidx.annotation.RequiresPermission
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import androidx.drawerlayout.widget.DrawerLayout
import com.google.android.material.navigation.NavigationView

class MainActivity : AppCompatActivity() {

    private lateinit var drawerLayout: DrawerLayout
    private lateinit var navView: NavigationView
    private lateinit var tvConnectionStatus: TextView
    private lateinit var btnScanBluetooth: Button
    private lateinit var btnScanWifi: Button
    private lateinit var lvBluetoothDevices: ListView
    private lateinit var lvWifiNetworks: ListView

    private lateinit var wifiManager: WifiManager
    private var bluetoothAdapter: BluetoothAdapter? = null
    private lateinit var bluetoothScanHandler: Handler

    private val bluetoothDevicesList = mutableListOf<String>()
    private val wifiNetworksList = mutableListOf<String>()

    private lateinit var bluetoothAdapterList: ArrayAdapter<String>
    private lateinit var wifiAdapterList: ArrayAdapter<String>

    private val REQUEST_BLUETOOTH_PERMISSIONS = 100
    private val REQUEST_WIFI_PERMISSIONS = 200

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // UI elements
        drawerLayout = findViewById(R.id.drawer_layout)
        navView = findViewById(R.id.nav_view)
        tvConnectionStatus = findViewById(R.id.tv_connection_status)
        btnScanBluetooth = findViewById(R.id.btn_scan_bluetooth)
        btnScanWifi = findViewById(R.id.btn_scan_wifi)
        lvBluetoothDevices = findViewById(R.id.lv_bluetooth_devices)
        lvWifiNetworks = findViewById(R.id.lv_wifi_networks)

        // Initialize adapters
        bluetoothAdapterList =
            ArrayAdapter(this, android.R.layout.simple_list_item_1, bluetoothDevicesList)
        wifiAdapterList =
            ArrayAdapter(this, android.R.layout.simple_list_item_1, wifiNetworksList)

        lvBluetoothDevices.adapter = bluetoothAdapterList
        lvWifiNetworks.adapter = wifiAdapterList

        // Managers
        wifiManager = applicationContext.getSystemService(Context.WIFI_SERVICE) as WifiManager
        val bluetoothManager = getSystemService(Context.BLUETOOTH_SERVICE) as BluetoothManager
        bluetoothAdapter = bluetoothManager.adapter

        bluetoothScanHandler = Handler(Looper.getMainLooper())

        // Button clicks
        btnScanBluetooth.setOnClickListener { startBluetoothScanSafe() }
        btnScanWifi.setOnClickListener { startWifiScanSafe() }
    }

    // ---------------- Permissions ----------------
    private fun hasBluetoothPermissions(): Boolean {
        val permissions = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            arrayOf(
                Manifest.permission.BLUETOOTH_SCAN,
                Manifest.permission.BLUETOOTH_CONNECT
            )
        } else {
            arrayOf(Manifest.permission.ACCESS_FINE_LOCATION) // needed pre-Android 12
        }
        return permissions.all {
            ContextCompat.checkSelfPermission(this, it) == PackageManager.PERMISSION_GRANTED
        }
    }

    private fun requestBluetoothPermissions() {
        val permissions = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            arrayOf(
                Manifest.permission.BLUETOOTH_SCAN,
                Manifest.permission.BLUETOOTH_CONNECT
            )
        } else {
            arrayOf(Manifest.permission.ACCESS_FINE_LOCATION)
        }
        ActivityCompat.requestPermissions(this, permissions, REQUEST_BLUETOOTH_PERMISSIONS)
    }

    private fun hasWifiPermissions(): Boolean {
        val permissions = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            arrayOf(
                Manifest.permission.ACCESS_FINE_LOCATION,
                Manifest.permission.NEARBY_WIFI_DEVICES
            )
        } else {
            arrayOf(Manifest.permission.ACCESS_FINE_LOCATION)
        }
        return permissions.all {
            ContextCompat.checkSelfPermission(this, it) == PackageManager.PERMISSION_GRANTED
        }
    }

    private fun requestWifiPermissions() {
        val permissions = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            arrayOf(
                Manifest.permission.ACCESS_FINE_LOCATION,
                Manifest.permission.NEARBY_WIFI_DEVICES
            )
        } else {
            arrayOf(Manifest.permission.ACCESS_FINE_LOCATION)
        }
        ActivityCompat.requestPermissions(this, permissions, REQUEST_WIFI_PERMISSIONS)
    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_SCAN)
    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == REQUEST_BLUETOOTH_PERMISSIONS) {
            if (grantResults.all { it == PackageManager.PERMISSION_GRANTED }) {
                startBluetoothScan()
            } else {
                Toast.makeText(this, "Bluetooth permissions required", Toast.LENGTH_SHORT).show()
            }
        }
        if (requestCode == REQUEST_WIFI_PERMISSIONS) {
            if (grantResults.all { it == PackageManager.PERMISSION_GRANTED }) {
                startWifiScanSafe()
            } else {
                Toast.makeText(this, "Wi-Fi permissions required", Toast.LENGTH_SHORT).show()
            }
        }
    }

    // ---------------- Bluetooth ----------------
    @RequiresPermission(Manifest.permission.BLUETOOTH_SCAN)
    private fun startBluetoothScanSafe() {
        if (hasBluetoothPermissions()) {
            startBluetoothScan()
        } else {
            requestBluetoothPermissions()
        }
    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_SCAN)
    private fun startBluetoothScan() {
        val adapter = bluetoothAdapter
        if (adapter == null) {
            Toast.makeText(this, "Bluetooth not supported on this device", Toast.LENGTH_SHORT).show()
            return
        }

        bluetoothDevicesList.clear()
        bluetoothAdapterList.notifyDataSetChanged()

        val scanner = adapter.bluetoothLeScanner
        if (scanner == null) {
            Toast.makeText(this, "Bluetooth scanner not available", Toast.LENGTH_SHORT).show()
            return
        }

        scanner.startScan(leScanCallback)

        // Stop scan after 10 seconds
        bluetoothScanHandler.postDelayed({
            try {
                scanner.stopScan(leScanCallback)
            } catch (e: Exception) {
                Log.e("MainActivity", "Failed to stop scan", e)
            }
        }, 10000)
    }

    private val leScanCallback = object : ScanCallback() {
        @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
        override fun onScanResult(callbackType: Int, result: ScanResult?) {
            result?.device?.let { device ->
                val nameOrAddress = device.name ?: device.address
                if (!bluetoothDevicesList.contains(nameOrAddress)) {
                    bluetoothDevicesList.add(nameOrAddress)
                    bluetoothAdapterList.notifyDataSetChanged()
                }
            }
        }
    }

    // ---------------- Wi-Fi ----------------
    private fun startWifiScanSafe() {
        if (hasWifiPermissions()) {
            startWifiScan()
        } else {
            requestWifiPermissions()
        }
    }

    private fun startWifiScan() {
        wifiNetworksList.clear()
        wifiAdapterList.notifyDataSetChanged()

        if (!wifiManager.isWifiEnabled) {
            Toast.makeText(this, "Wi-Fi is disabled", Toast.LENGTH_SHORT).show()
            return
        }

        val success = wifiManager.startScan()
        if (!success) {
            Toast.makeText(this, "Wi-Fi scan failed to start", Toast.LENGTH_SHORT).show()
            return
        }

        registerReceiver(
            wifiScanReceiver,
            IntentFilter(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION)
        )
    }

    private val wifiScanReceiver = object : BroadcastReceiver() {
        @RequiresPermission(Manifest.permission.ACCESS_FINE_LOCATION)
        override fun onReceive(context: Context?, intent: Intent?) {
            val results: List<WifiScanResult> = wifiManager.scanResults
            for (scanResult in results) {
                if (!wifiNetworksList.contains(scanResult.SSID) && scanResult.SSID.isNotEmpty()) {
                    wifiNetworksList.add(scanResult.SSID)
                }
            }
            wifiAdapterList.notifyDataSetChanged()
            unregisterReceiver(this)
        }
    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_SCAN)
    override fun onDestroy() {
        super.onDestroy()
        bluetoothAdapter?.bluetoothLeScanner?.stopScan(leScanCallback)
        bluetoothScanHandler.removeCallbacksAndMessages(null)

        try {
            unregisterReceiver(wifiScanReceiver)
        } catch (e: IllegalArgumentException) {
            Log.w("MainActivity", "Wifi receiver not registered")
        }
    }
}
