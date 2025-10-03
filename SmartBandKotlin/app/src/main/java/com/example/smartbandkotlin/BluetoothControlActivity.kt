package com.example.smartbandkotlin

import android.bluetooth.*
import android.content.Context
import android.os.Bundle
import android.util.Log
import android.widget.*
import androidx.appcompat.app.AppCompatActivity
import java.util.*

class BluetoothControlActivity : AppCompatActivity() {

    private var deviceAddress: String? = null
    private var bluetoothGatt: BluetoothGatt? = null
    private var writeCharacteristic: BluetoothGattCharacteristic? = null

    private lateinit var tvDeviceInfo: TextView
    private lateinit var etMessage: EditText
    private lateinit var etUsername: EditText
    private lateinit var btnSend: Button
    private lateinit var btnSendWifi: Button
    private lateinit var btnSendUsername: Button

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_bluetooth_control)

        tvDeviceInfo = findViewById(R.id.tv_device_info)
        etMessage = findViewById(R.id.et_message)
        btnSend = findViewById(R.id.btn_send)
        btnSendWifi = findViewById(R.id.btn_send_wifi)
        etUsername = findViewById(R.id.et_username)
        btnSendUsername = findViewById(R.id.btn_send_username)

        deviceAddress = intent.getStringExtra("BT_DEVICE_ADDRESS")
        tvDeviceInfo.text = "Device: $deviceAddress"

        if (deviceAddress != null && BluetoothAdapter.checkBluetoothAddress(deviceAddress)) {
            connectToDevice(deviceAddress!!)
        } else {
            Toast.makeText(this, "Invalid Bluetooth device address", Toast.LENGTH_SHORT).show()
            finish()
        }

        btnSend.setOnClickListener {
            val msg = etMessage.text.toString()
            if (msg.isNotEmpty()) {
                sendMessage(msg)
            }
        }

        btnSendUsername.setOnClickListener {
            val msg = "USER:" + etUsername.text.toString()
            if (msg.isNotEmpty()) {
                sendMessage(msg)
            }
        }

        btnSendWifi.setOnClickListener {
            showWifiDialog()
        }
    }

    private fun connectToDevice(address: String) {
        val adapter = BluetoothAdapter.getDefaultAdapter()
        val device: BluetoothDevice = adapter.getRemoteDevice(address)

        bluetoothGatt = device.connectGatt(this, false, gattCallback)
    }

    private val gattCallback = object : BluetoothGattCallback() {
        override fun onConnectionStateChange(gatt: BluetoothGatt, status: Int, newState: Int) {
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                Log.i("BT", "✅ Connected to GATT server")
                gatt.discoverServices()
            } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                Log.i("BT", "❌ Disconnected from GATT server")
            }
        }

        override fun onServicesDiscovered(gatt: BluetoothGatt, status: Int) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                for (service in gatt.services) {
                    Log.i("BT", "Service UUID: ${service.uuid}")
                    for (characteristic in service.characteristics) {
                        Log.i("BT", "  Characteristic UUID: ${characteristic.uuid}, props=${characteristic.properties}")
                        if (writeCharacteristic == null &&
                            (characteristic.properties and (BluetoothGattCharacteristic.PROPERTY_WRITE or BluetoothGattCharacteristic.PROPERTY_WRITE_NO_RESPONSE)) > 0
                        ) {
                            writeCharacteristic = characteristic
                            Log.i("BT", "✍️ Found writable characteristic: ${characteristic.uuid}")
                        }
                    }
                }
                runOnUiThread {
                    if (writeCharacteristic == null) {
                        Toast.makeText(this@BluetoothControlActivity, "No writable characteristic found!", Toast.LENGTH_SHORT).show()
                    } else {
                        Toast.makeText(this@BluetoothControlActivity, "Ready to send data", Toast.LENGTH_SHORT).show()
                    }
                }
            } else {
                Log.e("BT", "Service discovery failed: $status")
            }
        }
    }

    private fun sendMessage(msg: String) {
        if (writeCharacteristic == null || bluetoothGatt == null) {
            Toast.makeText(this, "Not ready to send", Toast.LENGTH_SHORT).show()
            return
        }
        writeCharacteristic!!.value = msg.toByteArray()
        val success = bluetoothGatt!!.writeCharacteristic(writeCharacteristic)
        if (success) {
            Toast.makeText(this, "Sent: $msg", Toast.LENGTH_SHORT).show()
        } else {
            Toast.makeText(this, "Send failed", Toast.LENGTH_SHORT).show()
        }
    }

    private fun showWifiDialog() {
        val layout = LinearLayout(this).apply {
            orientation = LinearLayout.VERTICAL
            setPadding(40, 20, 40, 10)
        }

        val etSsid = EditText(this).apply { hint = "Wi-Fi SSID" }
        val etPass = EditText(this).apply { hint = "Wi-Fi Password" }

        layout.addView(etSsid)
        layout.addView(etPass)

        android.app.AlertDialog.Builder(this)
            .setTitle("Send Wi-Fi Credentials")
            .setView(layout)
            .setPositiveButton("Send") { _, _ ->
                val ssid = etSsid.text.toString()
                val password = etPass.text.toString()
                if (ssid.isNotEmpty() && password.isNotEmpty()) {
                    val msg = "WIFI:$ssid,$password"
                    sendMessage(msg)
                }
            }
            .setNegativeButton("Cancel", null)
            .show()
    }

    override fun onDestroy() {
        super.onDestroy()
        bluetoothGatt?.close()
    }
}
