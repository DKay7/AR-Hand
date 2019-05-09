using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO.Ports;

public class movement : MonoBehaviour
{
    public float speed;
    private float amoutToMove;

    SerialPort sp = new SerialPort("COM3", 9600);
    // Start is called before the first frame update
    void Start()
    {
        sp.Open();
        sp.ReadTimeout = 1;

    }

    // Update is called once per frame
    void Update()
    {
        amoutToMove = speed * Time.deltaTime;

        if (sp.IsOpen)
        {
            try
            {
                MoveObject(sp.ReadByte());
                print(sp.ReadByte().ToString());

            }
            catch (System.Exception)
            {

            }
        }
    }
    void MoveObject(int Direction)
    {
        if (Direction == 65)
        {
            transform.Translate(Vector3.left * amoutToMove, Space.World);
        }

        if (Direction == 51)
        {
            transform.Translate(Vector3.right * amoutToMove, Space.World);
        }
        if (Direction == 54)
        {
            transform.Translate(Vector3.forward * amoutToMove, Space.World);
        }
        if (Direction == 66)
        {
            transform.Translate(Vector3.back * amoutToMove, Space.World);
        }
    }
}

