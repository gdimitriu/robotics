package io.github.gdimitriu.droidsimplecommand

import android.annotation.SuppressLint
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.text.Editable
import android.text.TextWatcher
import android.util.Log
import android.view.MotionEvent
import android.widget.Button
import android.widget.EditText
import kotlinx.coroutines.DelicateCoroutinesApi
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import java.io.OutputStreamWriter
import java.net.HttpURLConnection
import java.net.Socket
import java.net.URL

private const val TAG = "DroidSimpleCommand"
@DelicateCoroutinesApi
@Suppress("BlockingMethodInNonBlockingContext")
class MainActivity : AppCompatActivity() {

    private lateinit var forwardButton : Button
    private lateinit var backwardButton : Button
    private lateinit var leftButton : Button
    private lateinit var rightButton : Button
    private lateinit var stopButton: Button
    private lateinit var connectButton: Button;

    private lateinit var ipEditText: EditText
    private lateinit var portEditText: EditText
    private var socket: Socket? = null

    @SuppressLint("ClickableViewAccessibility")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        forwardButton = findViewById(R.id.forward)
        backwardButton = findViewById(R.id.backward)
        leftButton = findViewById(R.id.left)
        rightButton = findViewById(R.id.right)
        stopButton = findViewById(R.id.stop)
        ipEditText = findViewById(R.id.address)
        portEditText = findViewById(R.id.port)
        connectButton = findViewById(R.id.connect)

        var ipAddressValue = "192.168.4.1"
        var portValue = "8080"

        forwardButton.setOnTouchListener { view, motionEvent ->
            val event = motionEvent as MotionEvent
            if (event.actionMasked == MotionEvent.ACTION_DOWN) {
                Log.d(TAG, "Move forward")
                sendCommandToDroid("M1,0#\n")
                return@setOnTouchListener true
            } else if (event.actionMasked == MotionEvent.ACTION_UP) {
                Log.d(TAG, "Stop")
                sendCommandToDroid("M0,0#\n")
                return@setOnTouchListener true
            }
            return@setOnTouchListener false
        }
        backwardButton.setOnTouchListener { view, motionEvent ->
            val event = motionEvent as MotionEvent
            if (event.actionMasked == MotionEvent.ACTION_DOWN) {
                Log.d(TAG, "Move backward")
                sendCommandToDroid("M-1,0#\n")
                return@setOnTouchListener true
            } else if (event.actionMasked == MotionEvent.ACTION_UP) {
                Log.d(TAG, "Stop")
                sendCommandToDroid("M0,0#\n")
                return@setOnTouchListener true
            }
            return@setOnTouchListener false
        }
        leftButton.setOnTouchListener { view, motionEvent ->
            val event = motionEvent as MotionEvent
            if (event.actionMasked == MotionEvent.ACTION_DOWN) {
                Log.d(TAG, "Move left")
                sendCommandToDroid("M0,-1#\n")
                return@setOnTouchListener true
            } else if (event.actionMasked == MotionEvent.ACTION_UP) {
                Log.d(TAG, "Stop")
                sendCommandToDroid("M0,0#\n")
                return@setOnTouchListener true
            }
            return@setOnTouchListener false
        }
        rightButton.setOnTouchListener { view, motionEvent ->
            val event = motionEvent as MotionEvent
            if (event.actionMasked == MotionEvent.ACTION_DOWN) {
                Log.d(TAG, "Move right")
                sendCommandToDroid("M0,1#\n")
                return@setOnTouchListener true
            } else if (event.actionMasked == MotionEvent.ACTION_UP) {
                Log.d(TAG, "Stop")
                sendCommandToDroid("M0,0#\n")
                return@setOnTouchListener true
            }
            return@setOnTouchListener false
        }
        stopButton.setOnClickListener { view ->
            Log.d(TAG, "Full stop")
            sendCommandToDroid("b#\n")
        }
        connectButton.setOnClickListener { view ->
            Log.d(TAG, "Connect to droid")
            GlobalScope.launch {
                socket = Socket(ipAddressValue, portValue.toInt())
                socket?.tcpNoDelay = true
            }
        }

        val portWatcher = object : TextWatcher {
            override fun beforeTextChanged(sequence: CharSequence?, start: Int, count: Int, after: Int) {
                //
            }

            override fun onTextChanged(sequence: CharSequence?, start: Int, before: Int, count: Int) {
                portValue = sequence.toString()
            }

            override fun afterTextChanged(sequence: Editable?) {
                //
            }
        }
        portEditText.addTextChangedListener(portWatcher)

        val addressWatcher = object : TextWatcher {
            override fun beforeTextChanged(sequence: CharSequence?, start: Int, count: Int, after: Int) {
                //
            }

            override fun onTextChanged(sequence: CharSequence?, start: Int, before: Int, count: Int) {
                ipAddressValue = sequence.toString()
            }

            override fun afterTextChanged(sequence: Editable?) {
                //
            }
        }
        ipEditText.addTextChangedListener(addressWatcher)
    }

    private fun sendCommandToDroid(message: String) {
        GlobalScope.launch {
            if (socket != null) {
                val outputStreamWriter = OutputStreamWriter(socket!!.getOutputStream())
                outputStreamWriter.write(message)
                outputStreamWriter.flush()
            }
        }
    }
    /*
    this is not used
    private fun sendHttpConnection() {
        GlobalScope.launch {
                var url = URL("http://${ipAddressValue}:${portValue}")
                var httpURLConnection = url.openConnection() as HttpURLConnection
                httpURLConnection.doOutput = true
                httpURLConnection.doInput = true
                val outputStreamWriter = OutputStreamWriter(httpURLConnection.outputStream)
                outputStreamWriter.write("b#\n")
                outputStreamWriter.flush()
                outputStreamWriter.close()
                Log.e(TAG, httpURLConnection.responseCode.toString())
                httpURLConnection.disconnect()
        }
    }
     */
}