package com.example.smartbandkotlin

import android.Manifest
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.content.pm.PackageManager
import android.os.Bundle
import android.widget.ArrayAdapter
import android.widget.Button
import android.widget.ListView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat

class MainActivity : AppCompatActivity() {

    private lateinit var lvBluetoothDevices: ListView
    private lateinit var btnScan: Button
    private lateinit var bluetoothAdapter: BluetoothAdapter
    private lateinit var bluetoothDevicesList: ArrayList<String>
    private lateinit var devicesAdapter: ArrayAdapter<String>

    private val REQUEST_BLUETOOTH_PERMISSIONS = 1

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        lvBluetoothDevices = findViewById(R.id.lv_bluetooth_devices)
        btnScan = findViewById(R.id.btn_scan)

        bluetoothAdapter = BluetoothAdapter.getDefaultAdapter()
        bluetoothDevicesList = ArrayList()
        devicesAdapter = ArrayAdapter(this, android.R.layout.simple_list_item_1, bluetoothDevicesList)
        lvBluetoothDevices.adapter = devicesAdapter

        // request permissions
        checkPermissions()

        // scan button
        btnScan.setOnClickListener {
            scanDevices()
        }

        // click list item → go to BluetoothControlActivity
        lvBluetoothDevices.setOnItemClickListener { _, _, position, _ ->
            val deviceInfo = bluetoothDevicesList[position]
            val address = deviceInfo.substringAfter("\n") // after newline is the MAC address

            val intent = Intent(this, BluetoothControlActivity::class.java)
            intent.putExtra("BT_DEVICE_ADDRESS", address)
            startActivity(intent)
        }
    }

    private fun checkPermissions() {
        val permissions = arrayOf(
            Manifest.permission.BLUETOOTH_SCAN,
            Manifest.permission.BLUETOOTH_CONNECT,
            Manifest.permission.ACCESS_FINE_LOCATION
        )

        val needed = permissions.filter {
            ContextCompat.checkSelfPermission(this, it) != PackageManager.PERMISSION_GRANTED
        }

        if (needed.isNotEmpty()) {
            ActivityCompat.requestPermissions(this, needed.toTypedArray(), REQUEST_BLUETOOTH_PERMISSIONS)
        }
    }

    private fun scanDevices() {
        bluetoothDevicesList.clear()
        devicesAdapter.notifyDataSetChanged()

        if (bluetoothAdapter.isEnabled) {
            // Add already paired devices
            val pairedDevices: Set<BluetoothDevice>? = bluetoothAdapter.bondedDevices
            pairedDevices?.forEach { device ->
                bluetoothDevicesList.add("${device.name ?: "Unknown"}\n${device.address}")
            }

            // Start discovery for new devices
            if (bluetoothAdapter.isDiscovering) {
                bluetoothAdapter.cancelDiscovery()
            }
            bluetoothAdapter.startDiscovery()

            Toast.makeText(this, "Scanning for devices...", Toast.LENGTH_SHORT).show()
        } else {
            Toast.makeText(this, "Bluetooth is disabled", Toast.LENGTH_SHORT).show()
        }
    }

    // Receiver to get discovered devices
    private val receiver = object : BroadcastReceiver() {
        override fun onReceive(context: Context?, intent: Intent?) {
            if (intent?.action == BluetoothDevice.ACTION_FOUND) {
                val device: BluetoothDevice? =
                    intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE)

                device?.let {
                    val deviceInfo = "${it.name ?: "Unknown"}\n${it.address}"
                    if (!bluetoothDevicesList.contains(deviceInfo)) {
                        bluetoothDevicesList.add(deviceInfo)
                        devicesAdapter.notifyDataSetChanged()
                    }
                }
            }
        }
    }

    override fun onResume() {
        super.onResume()
        val filter = IntentFilter(BluetoothDevice.ACTION_FOUND)
        registerReceiver(receiver, filter)
    }

    override fun onPause() {
        super.onPause()
        unregisterReceiver(receiver)
    }
}
