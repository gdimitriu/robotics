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
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import java.io.OutputStreamWriter
import java.net.HttpURLConnection
import java.net.Socket
import java.net.URL

private const val TAG = "DroidSimpleCommand"
class MainActivity : AppCompatActivity() {

    private lateinit var forwardButton : Button
    private lateinit var backwardButton : Button
    private lateinit var leftButton : Button
    private lateinit var rightButton : Button
    private lateinit var stopButton: Button

    private lateinit var ipEditText: EditText
    private lateinit var portEditText: EditText

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

        var ipAddressValue : String = "192.168.4.1"
        var portValue : String = "8080"

        forwardButton.setOnTouchListener { view, motionEvent ->
            var event = motionEvent as MotionEvent
            if (event.actionMasked == MotionEvent.ACTION_DOWN) {
                Log.e(TAG, "Move forward")
                GlobalScope.launch() {
                    val s:Socket = Socket(ipAddressValue, portValue.toInt())
                    s.tcpNoDelay= true
                    val outputStreamWriter = OutputStreamWriter(s.getOutputStream())
                    outputStreamWriter.write("M1,0#\n")
                    outputStreamWriter.flush()
                    outputStreamWriter.close()
                    s.close()
                }
                return@setOnTouchListener true
            } else if (event.actionMasked == MotionEvent.ACTION_UP) {
                Log.e(TAG, "Stop")
                GlobalScope.launch() {
                    val s:Socket = Socket(ipAddressValue, portValue.toInt())
                    s.tcpNoDelay= true
                    val outputStreamWriter = OutputStreamWriter(s.getOutputStream())
                    outputStreamWriter.write("M0,0#\n")
                    outputStreamWriter.flush()
                    outputStreamWriter.close()
                    s.close()
                }
                return@setOnTouchListener true
            }
            return@setOnTouchListener false
        }
        backwardButton.setOnTouchListener { view, motionEvent ->
            var event = motionEvent as MotionEvent
            if (event.actionMasked == MotionEvent.ACTION_DOWN) {
                Log.e(TAG, "Move backward")
                GlobalScope.launch() {
                    val s:Socket = Socket(ipAddressValue, portValue.toInt())
                    s.tcpNoDelay= true
                    val outputStreamWriter = OutputStreamWriter(s.getOutputStream())
                    outputStreamWriter.write("M-1,0#\n")
                    outputStreamWriter.flush()
                    outputStreamWriter.close()
                    s.close()
                }
                return@setOnTouchListener true
            } else if (event.actionMasked == MotionEvent.ACTION_UP) {
                Log.e(TAG, "Stop")
                GlobalScope.launch() {
                    val s:Socket = Socket(ipAddressValue, portValue.toInt())
                    s.tcpNoDelay= true
                    val outputStreamWriter = OutputStreamWriter(s.getOutputStream())
                    outputStreamWriter.write("M0,0#\n")
                    outputStreamWriter.flush()
                    outputStreamWriter.close()
                    s.close()
                }
                return@setOnTouchListener true
            }
            return@setOnTouchListener false
        }
        leftButton.setOnTouchListener { view, motionEvent ->
            var event = motionEvent as MotionEvent
            if (event.actionMasked == MotionEvent.ACTION_DOWN) {
                Log.e(TAG, "Move left")
                GlobalScope.launch() {
                    val s:Socket = Socket(ipAddressValue, portValue.toInt())
                    s.tcpNoDelay= true
                    val outputStreamWriter = OutputStreamWriter(s.getOutputStream())
                    outputStreamWriter.write("M0,-1#\n")
                    outputStreamWriter.flush()
                    outputStreamWriter.close()
                    s.close()
                }
                return@setOnTouchListener true
            } else if (event.actionMasked == MotionEvent.ACTION_UP) {
                Log.e(TAG, "Stop")
                GlobalScope.launch() {
                    val s:Socket = Socket(ipAddressValue, portValue.toInt())
                    s.tcpNoDelay= true
                    val outputStreamWriter = OutputStreamWriter(s.getOutputStream())
                    outputStreamWriter.write("M0,0#\n")
                    outputStreamWriter.flush()
                    outputStreamWriter.close()
                    s.close()                }
                return@setOnTouchListener true
            }
            return@setOnTouchListener false
        }
        rightButton.setOnTouchListener { view, motionEvent ->
            var event = motionEvent as MotionEvent
            if (event.actionMasked == MotionEvent.ACTION_DOWN) {
                Log.e(TAG, "Move right")
                GlobalScope.launch() {
                    val s:Socket = Socket(ipAddressValue, portValue.toInt())
                    s.tcpNoDelay= true
                    val outputStreamWriter = OutputStreamWriter(s.getOutputStream())
                    outputStreamWriter.write("M0,1#\n")
                    outputStreamWriter.flush()
                    outputStreamWriter.close()
                    s.close()
                }
                return@setOnTouchListener true
            } else if (event.actionMasked == MotionEvent.ACTION_UP) {
                Log.e(TAG, "Stop")
                GlobalScope.launch() {
                    val s:Socket = Socket(ipAddressValue, portValue.toInt())
                    s.tcpNoDelay= true
                    val outputStreamWriter = OutputStreamWriter(s.getOutputStream())
                    outputStreamWriter.write("M0,0#\n")
                    outputStreamWriter.flush()
                    outputStreamWriter.close()
                    s.close()
                }
                return@setOnTouchListener true
            }
            return@setOnTouchListener false
        }
        stopButton.setOnClickListener { view ->
            Log.e(TAG, "Full stop")
            GlobalScope.launch() {

                val s:Socket = Socket(ipAddressValue, portValue.toInt())
                s.tcpNoDelay= true
                val outputStreamWriter = OutputStreamWriter(s.getOutputStream())
                outputStreamWriter.write("b#\n")
                outputStreamWriter.flush()
                outputStreamWriter.close()
                s.close()
/*                var url = URL("http://${ipAddressValue}:${portValue}")
                var httpURLConnection = url.openConnection() as HttpURLConnection
                httpURLConnection.doOutput = true
                httpURLConnection.doInput = true
                val outputStreamWriter = OutputStreamWriter(httpURLConnection.outputStream)
                outputStreamWriter.write("b#\n")
                outputStreamWriter.flush()
                outputStreamWriter.close()
                Log.e(TAG, httpURLConnection.responseCode.toString())
                httpURLConnection.disconnect() */
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
}