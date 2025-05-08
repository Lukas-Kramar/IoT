import { Button, Card, Col, Row } from "react-bootstrap";
import { MeasurementPoint, Sensor } from "../../../../../API/requests/measurementPointsRequests";
import { memo, useEffect, useState } from "react";
import { DashboardModalVersion } from "../../Dashboard";
import SensorItem from "./SensorItem";
import TemperatureChart from "../../../../components/charts/TemperatureChart";

interface Props {
    measurementPoint: MeasurementPoint,
    setModalVersion: React.Dispatch<React.SetStateAction<DashboardModalVersion>>,
    setEditedMeasurementPoint: React.Dispatch<React.SetStateAction<MeasurementPoint | null>>,
}
const MeasurementPointCard = (props: Props) => {
    const {
        measurementPoint,
        setModalVersion,
        setEditedMeasurementPoint
    } = props;

    const [isLoading, setIsLoading] = useState(false);[]
    const [alerts, setAlerts] = useState<string[]>([]);

    const [sensors, setSensors] = useState<Sensor[]>(measurementPoint.sensors || []);

    // useEffect(() => {
    //     const fetchSensors = async () => {
    //         try {

    //         } catch (err) {

    //         } finally
    //     }
    // }, []);


    return (
        <Row className="shadow border border-3 border-secondary  rounded-3 mb-3 p-3">
            <Col sm={10}>
                <h4 className="text-primary">{measurementPoint.name}</h4>
                <p className="text-muted">MeasurementPoint ID: {measurementPoint._id}</p>
                <p>{measurementPoint.description}</p>
            </Col>
            <Col sm={2} className="d-flex flex-row justify-content-end gap-2 align-items-start">
                <Button
                    variant="warning"
                    onClick={() => {
                        setModalVersion('update-measurement-point');
                        setEditedMeasurementPoint(measurementPoint);
                    }}
                >
                    <i className="bi bi-pencil-fill" />
                    <span className="ms-1">Edit</span>
                </Button>
                <Button
                    variant="danger"
                    onClick={() => {
                        setModalVersion('delete-measurement-point');
                        setEditedMeasurementPoint(measurementPoint);
                    }}
                >
                    <i className="bi bi-trash" />
                    <span className="ms-1">Delete</span>
                </Button>
            </Col>


            <Col sm={12} className="d-flex flex-column gap-4">
                <hr className="mb-0"/>
                <div>
                    <p>
                        <strong>Temperature data recorded in last 24h:</strong>
                    </p>
                    <TemperatureChart data={[]} />
                </div>
                <div>
                    <p>   <strong className="">Measurement Point Sensors:</strong></p>
                    {sensors.map((sensor) => (
                        <SensorItem sensor={sensor} />
                    ))}
                </div>
            </Col>
        </Row>
    );
}

export default memo(MeasurementPointCard);